#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <map>
#include <numeric>
#include <random>
#include <string>
#include <vector>

#include "orderbook/orderBook.h"

namespace {

using Clock = std::chrono::steady_clock;

struct Config {
    std::size_t passiveOps = 1000000;
    std::size_t cancelOps = 1000000;
    std::size_t matchingOps = 1000000;
    std::size_t mixedOps = 5000000;
    std::size_t latencyOps = 200000;
    std::size_t warmupOps = 100000;
    std::size_t trials = 10;
    std::uint32_t seed = 123456789u;
};

enum class OpType {
    PassiveLimit,
    Cancel,
    CrossingLimit,
    Market,
};

struct BenchOp {
    OpType type;
    Side side;
    int qty;
    int price;
    int id;
};

struct ThroughputStats {
    std::string scenario;
    std::size_t operations = 0;
    double medianSeconds = 0.0;
    double medianOpsPerSec = 0.0;
    double medianMops = 0.0;
    double medianNsPerOp = 0.0;
    double minMops = 0.0;
    double maxMops = 0.0;
};

struct LatencyStats {
    std::string scenario;
    std::size_t operations = 0;
    double totalSeconds = 0.0;
    double p50Ns = 0.0;
    double p95Ns = 0.0;
    double p99Ns = 0.0;
};

struct IdGenerator {
    int nextId = 1;

    int next() {
        return nextId++;
    }
};

std::uint64_t gSink = 0;

[[noreturn]] void failBenchmark(const std::string& message) {
    std::cerr << "Benchmark validation failed: " << message << "\n";
    std::exit(1);
}

void requireBenchmark(bool condition, const std::string& message) {
    if (!condition) {
        failBenchmark(message);
    }
}

template <typename T>
double medianOf(std::vector<T> values) {
    assert(!values.empty());
    std::sort(values.begin(), values.end());
    const std::size_t n = values.size();
    if ((n % 2U) == 1U) {
        return static_cast<double>(values[n / 2U]);
    }
    return 0.5 * (static_cast<double>(values[(n / 2U) - 1U]) + static_cast<double>(values[n / 2U]));
}

double percentileFromSorted(const std::vector<std::int64_t>& sorted, double percentile) {
    assert(!sorted.empty());
    const double idx = percentile * static_cast<double>(sorted.size() - 1U);
    const std::size_t lo = static_cast<std::size_t>(idx);
    const std::size_t hi = std::min(lo + 1U, sorted.size() - 1U);
    const double frac = idx - static_cast<double>(lo);
    return (1.0 - frac) * static_cast<double>(sorted[lo]) + frac * static_cast<double>(sorted[hi]);
}

std::uint64_t consumeBookState(OrderBook& book) {
    std::uint64_t h = 1469598103934665603ULL;
    auto mix = [&h](std::uint64_t v) {
        h ^= v;
        h *= 1099511628211ULL;
    };

    mix(static_cast<std::uint64_t>(book.getIds().idStore.size()));
    mix(static_cast<std::uint64_t>(book.getPrices().buyPriceLevelStorage.size()));
    mix(static_cast<std::uint64_t>(book.getPrices().sellPriceLevelStorage.size()));

    if (!book.getPrices().isBuyStoreEmpty()) {
        mix(static_cast<std::uint64_t>(book.getPrices().bestBidPrice() + 1000000));
    }
    if (!book.getPrices().isSellStoreEmpty()) {
        mix(static_cast<std::uint64_t>(book.getPrices().bestAskPrice() + 1000000));
    }

    const std::size_t maxNodesToSample = std::min<std::size_t>(book.getNodes().store.size(), 32U);
    for (std::size_t i = 0; i < maxNodesToSample; ++i) {
        const orderNode& node = book.getNodes().store[i];
        mix(static_cast<std::uint64_t>(node.id + 1000000));
        mix(static_cast<std::uint64_t>(node.qty + 1000000));
        mix(static_cast<std::uint64_t>(node.active ? 1 : 0));
    }

    gSink ^= h;
    return h;
}

bool validateLevelChain(const std::map<int, std::vector<int>>& levels, const NodeStorage& nodes, Side side) {
    (void)side;
    for (const auto& kv : levels) {
        const int price = kv.first;
        const std::vector<int>& headTail = kv.second;
        if (headTail.size() < 2U) {
            return false;
        }

        const int head = headTail[0];
        const int tail = headTail[1];
        if (head < 0 || tail < 0) {
            return false;
        }
        if (head >= static_cast<int>(nodes.store.size()) || tail >= static_cast<int>(nodes.store.size())) {
            return false;
        }

        const orderNode& headNode = nodes.store[static_cast<std::size_t>(head)];
        const orderNode& tailNode = nodes.store[static_cast<std::size_t>(tail)];
        if (static_cast<int>(headNode.price) != price || static_cast<int>(tailNode.price) != price) {
            return false;
        }
    }

    return true;
}

bool validateBook(OrderBook& book) {
    const NodeStorage& nodes = book.getNodes();

    for (const auto& kv : book.getIds().idStore) {
        const int id = kv.first;
        const int idx = kv.second;
        if (idx < 0 || idx >= static_cast<int>(nodes.store.size())) {
            return false;
        }
        const orderNode& node = nodes.store[static_cast<std::size_t>(idx)];
        if (node.id != id) {
            return false;
        }
    }

    if (!validateLevelChain(book.getPrices().buyPriceLevelStorage, nodes, Side::BUY)) {
        return false;
    }
    if (!validateLevelChain(book.getPrices().sellPriceLevelStorage, nodes, Side::SELL)) {
        return false;
    }

    return true;
}

std::vector<LimitOrder> makePassiveNonCrossingOrders(std::size_t count, IdGenerator& ids) {
    std::vector<LimitOrder> orders;
    orders.reserve(count);

    for (std::size_t i = 0; i < count; ++i) {
        const bool buySide = ((i % 2U) == 0U);
        if (buySide) {
            const int price = 94 + static_cast<int>((i / 2U) % 5U);  // 94..98
            orders.push_back(LimitOrder{ids.next(), OrderType::LIMIT, Side::BUY, 10, static_cast<double>(price)});
        } else {
            const int price = 101 + static_cast<int>((i / 2U) % 6U); // 101..106
            orders.push_back(LimitOrder{ids.next(), OrderType::LIMIT, Side::SELL, 10, static_cast<double>(price)});
        }
    }

    return orders;
}

std::vector<LimitOrder> makeCancelPopulationOrders(std::size_t count, IdGenerator& ids) {
    std::vector<LimitOrder> orders;
    orders.reserve(count);

    for (std::size_t i = 0; i < count; ++i) {
        const int price = 90 + static_cast<int>(i % 11U); // 90..100
        orders.push_back(LimitOrder{ids.next(), OrderType::LIMIT, Side::BUY, 5, static_cast<double>(price)});
    }

    return orders;
}

std::vector<int> makeCancellationIdsFromPopulation(const std::vector<LimitOrder>& population, std::uint32_t seed) {
    std::vector<int> ids;
    ids.reserve(population.size());
    for (const LimitOrder& o : population) {
        ids.push_back(o.id);
    }
    std::mt19937 rng(seed);
    std::shuffle(ids.begin(), ids.end(), rng);
    return ids;
}

std::vector<LimitOrder> makeMatchingPreloadAsks(std::size_t matchingOps, IdGenerator& ids) {
    std::vector<LimitOrder> asks;
    const std::size_t totalQtyNeeded = matchingOps + 10000U;
    const int qtyPerOrder = 100;
    const std::size_t orderCount = (totalQtyNeeded + static_cast<std::size_t>(qtyPerOrder - 1)) / static_cast<std::size_t>(qtyPerOrder);
    asks.reserve(orderCount);

    for (std::size_t i = 0; i < orderCount; ++i) {
        const int price = 101 + static_cast<int>(i % 11U); // 101..111
        asks.push_back(LimitOrder{ids.next(), OrderType::LIMIT, Side::SELL, qtyPerOrder, static_cast<double>(price)});
    }

    return asks;
}

std::vector<LimitOrder> makeCrossingBuyLimits(std::size_t count, IdGenerator& ids) {
    std::vector<LimitOrder> orders;
    orders.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
        orders.push_back(LimitOrder{ids.next(), OrderType::LIMIT, Side::BUY, 1, 120.0});
    }
    return orders;
}

struct MixedWorkload {
    std::vector<LimitOrder> preloadLiquidity;
    std::vector<LimitOrder> preloadCancelable;
    std::vector<BenchOp> ops;
};

MixedWorkload makeMixedWorkload(const Config& cfg, IdGenerator& ids) {
    MixedWorkload w;

    const std::size_t n = cfg.mixedOps;
    const std::size_t passiveCount = (n * 40U) / 100U;
    const std::size_t cancelCount = (n * 20U) / 100U;
    const std::size_t crossingCount = (n * 20U) / 100U;
    const std::size_t marketCount = n - passiveCount - cancelCount - crossingCount;

    const std::size_t aggressiveCount = crossingCount + marketCount;
    const std::size_t perSideQty = (aggressiveCount / 2U) + 100000U;

    const int preloadQtyPerOrder = 100;
    const std::size_t askOrderCount = (perSideQty + static_cast<std::size_t>(preloadQtyPerOrder - 1)) / static_cast<std::size_t>(preloadQtyPerOrder);
    const std::size_t bidOrderCount = askOrderCount;

    w.preloadLiquidity.reserve(askOrderCount + bidOrderCount);
    for (std::size_t i = 0; i < askOrderCount; ++i) {
        const int askPrice = 101 + static_cast<int>(i % 11U); // 101..111
        w.preloadLiquidity.push_back(LimitOrder{ids.next(), OrderType::LIMIT, Side::SELL, preloadQtyPerOrder, static_cast<double>(askPrice)});
    }
    for (std::size_t i = 0; i < bidOrderCount; ++i) {
        const int bidPrice = 99 - static_cast<int>(i % 11U); // 99..89
        w.preloadLiquidity.push_back(LimitOrder{ids.next(), OrderType::LIMIT, Side::BUY, preloadQtyPerOrder, static_cast<double>(bidPrice)});
    }

    w.preloadCancelable.reserve(cancelCount);
    std::vector<int> cancelPoolIds;
    cancelPoolIds.reserve(cancelCount);
    for (std::size_t i = 0; i < cancelCount; ++i) {
        const bool buy = ((i % 2U) == 0U);
        const int price = buy ? 70 : 130;
        const int id = ids.next();
        w.preloadCancelable.push_back(LimitOrder{id, OrderType::LIMIT, buy ? Side::BUY : Side::SELL, 1, static_cast<double>(price)});
        cancelPoolIds.push_back(id);
    }

    std::vector<OpType> opKinds;
    opKinds.reserve(n);
    opKinds.insert(opKinds.end(), passiveCount, OpType::PassiveLimit);
    opKinds.insert(opKinds.end(), cancelCount, OpType::Cancel);
    opKinds.insert(opKinds.end(), crossingCount, OpType::CrossingLimit);
    opKinds.insert(opKinds.end(), marketCount, OpType::Market);

    std::mt19937 rng(cfg.seed);
    std::shuffle(opKinds.begin(), opKinds.end(), rng);

    w.ops.reserve(n);
    std::size_t cancelCursor = 0;
    std::size_t passiveCursor = 0;
    std::size_t crossingCursor = 0;
    std::size_t marketCursor = 0;

    for (const OpType kind : opKinds) {
        if (kind == OpType::PassiveLimit) {
            const bool buy = ((passiveCursor % 2U) == 0U);
            const int price = buy
                ? (94 + static_cast<int>((passiveCursor / 2U) % 5U))
                : (101 + static_cast<int>((passiveCursor / 2U) % 6U));
            w.ops.push_back(BenchOp{OpType::PassiveLimit, buy ? Side::BUY : Side::SELL, 1, price, ids.next()});
            ++passiveCursor;
        } else if (kind == OpType::Cancel) {
            requireBenchmark(cancelCursor < cancelPoolIds.size(), "insufficient cancel IDs during mixed workload generation");
            w.ops.push_back(BenchOp{OpType::Cancel, Side::BUY, 0, 0, cancelPoolIds[cancelCursor++]});
        } else if (kind == OpType::CrossingLimit) {
            const bool buy = ((crossingCursor % 2U) == 0U);
            const int price = buy ? 120 : 80;
            w.ops.push_back(BenchOp{OpType::CrossingLimit, buy ? Side::BUY : Side::SELL, 1, price, ids.next()});
            ++crossingCursor;
        } else {
            const bool buy = ((marketCursor % 2U) == 0U);
            w.ops.push_back(BenchOp{OpType::Market, buy ? Side::BUY : Side::SELL, 1, 0, ids.next()});
            ++marketCursor;
        }
    }

    return w;
}

void applyPreload(OrderBook& book, const std::vector<LimitOrder>& orders) {
    for (const LimitOrder& order : orders) {
        book.getMatching().checkLimitOrder(order);
    }
}

void warmupPassive(const std::vector<LimitOrder>& orders, std::size_t warmupOps) {
    OrderBook warm;
    const std::size_t n = std::min(warmupOps, orders.size());
    for (std::size_t i = 0; i < n; ++i) {
        warm.getMatching().checkLimitOrder(orders[i]);
    }
    requireBenchmark(validateBook(warm), "warmup passive produced invalid book state");
    consumeBookState(warm);
}

void warmupCancel(const std::vector<LimitOrder>& population, const std::vector<int>& cancelIds, std::size_t warmupOps) {
    OrderBook warm;
    applyPreload(warm, population);
    const std::size_t n = std::min(warmupOps, cancelIds.size());
    for (std::size_t i = 0; i < n; ++i) {
        warm.getCancel().cancelOrder(cancelIds[i]);
    }
    requireBenchmark(validateBook(warm), "warmup cancellation produced invalid book state");
    consumeBookState(warm);
}

void warmupMatching(const std::vector<LimitOrder>& preloadedAsks, const std::vector<LimitOrder>& crossingOrders, std::size_t warmupOps) {
    OrderBook warm;
    applyPreload(warm, preloadedAsks);
    const std::size_t n = std::min(warmupOps, crossingOrders.size());
    for (std::size_t i = 0; i < n; ++i) {
        warm.getMatching().checkLimitOrder(crossingOrders[i]);
    }
    requireBenchmark(validateBook(warm), "warmup matching produced invalid book state");
    consumeBookState(warm);
}

void warmupMixed(const MixedWorkload& w, std::size_t warmupOps) {
    OrderBook warm;
    applyPreload(warm, w.preloadLiquidity);
    applyPreload(warm, w.preloadCancelable);

    const std::size_t n = std::min(warmupOps, w.ops.size());
    for (std::size_t i = 0; i < n; ++i) {
        const BenchOp& op = w.ops[i];
        switch (op.type) {
            case OpType::PassiveLimit:
                warm.getMatching().checkLimitOrder(
                    LimitOrder{op.id, OrderType::LIMIT, op.side, op.qty, static_cast<double>(op.price)});
                break;
            case OpType::Cancel:
                warm.getCancel().cancelOrder(op.id);
                break;
            case OpType::CrossingLimit:
                warm.getMatching().checkLimitOrder(
                    LimitOrder{op.id, OrderType::LIMIT, op.side, op.qty, static_cast<double>(op.price)});
                break;
            case OpType::Market:
                warm.getMatching().checkMarketOrder(
                    MarketOrder{op.id, OrderType::MARKET, op.side, op.qty});
                break;
        }
    }

    requireBenchmark(validateBook(warm), "warmup mixed produced invalid book state");
    consumeBookState(warm);
}

ThroughputStats runPassiveThroughput(const Config& cfg, const std::vector<LimitOrder>& orders) {
    std::vector<double> secs;
    std::vector<double> mops;
    secs.reserve(cfg.trials);
    mops.reserve(cfg.trials);

    for (std::size_t t = 0; t < cfg.trials; ++t) {
        OrderBook book;

        const auto start = Clock::now();
        for (const LimitOrder& order : orders) {
            book.getMatching().checkLimitOrder(order);
        }
        const auto end = Clock::now();

        const std::chrono::duration<double> d = end - start;
        const double seconds = d.count();
        const double opsPerSec = static_cast<double>(orders.size()) / seconds;
        secs.push_back(seconds);
        mops.push_back(opsPerSec / 1e6);

        requireBenchmark(validateBook(book), "passive trial produced invalid book state");
        requireBenchmark(book.getIds().idStore.size() == orders.size(), "passive trial id store size mismatch");
        consumeBookState(book);
    }

    ThroughputStats s;
    s.scenario = "Passive insertion";
    s.operations = orders.size();
    s.medianSeconds = medianOf(secs);
    s.medianMops = medianOf(mops);
    s.medianOpsPerSec = s.medianMops * 1e6;
    s.medianNsPerOp = (s.medianSeconds / static_cast<double>(orders.size())) * 1e9;
    s.minMops = *std::min_element(mops.begin(), mops.end());
    s.maxMops = *std::max_element(mops.begin(), mops.end());
    return s;
}

ThroughputStats runCancellationThroughput(
    const Config& cfg,
    const std::vector<LimitOrder>& population,
    const std::vector<int>& cancelIds) {

    std::vector<double> secs;
    std::vector<double> mops;
    secs.reserve(cfg.trials);
    mops.reserve(cfg.trials);

    for (std::size_t t = 0; t < cfg.trials; ++t) {
        OrderBook book;
        applyPreload(book, population);

        const auto start = Clock::now();
        for (int id : cancelIds) {
            book.getCancel().cancelOrder(id);
        }
        const auto end = Clock::now();

        const std::chrono::duration<double> d = end - start;
        const double seconds = d.count();
        const double opsPerSec = static_cast<double>(cancelIds.size()) / seconds;
        secs.push_back(seconds);
        mops.push_back(opsPerSec / 1e6);

        requireBenchmark(validateBook(book), "cancellation trial produced invalid book state");
        requireBenchmark(book.getIds().idStore.empty(), "cancellation trial left active IDs");
        requireBenchmark(book.getPrices().isBuyStoreEmpty(), "cancellation trial left buy levels");
        requireBenchmark(book.getPrices().isSellStoreEmpty(), "cancellation trial left sell levels");
        consumeBookState(book);
    }

    ThroughputStats s;
    s.scenario = "Cancellation";
    s.operations = cancelIds.size();
    s.medianSeconds = medianOf(secs);
    s.medianMops = medianOf(mops);
    s.medianOpsPerSec = s.medianMops * 1e6;
    s.medianNsPerOp = (s.medianSeconds / static_cast<double>(cancelIds.size())) * 1e9;
    s.minMops = *std::min_element(mops.begin(), mops.end());
    s.maxMops = *std::max_element(mops.begin(), mops.end());
    return s;
}

ThroughputStats runMatchingThroughput(
    const Config& cfg,
    const std::vector<LimitOrder>& preloadedAsks,
    const std::vector<LimitOrder>& crossingOrders) {

    std::vector<double> secs;
    std::vector<double> mops;
    secs.reserve(cfg.trials);
    mops.reserve(cfg.trials);

    for (std::size_t t = 0; t < cfg.trials; ++t) {
        OrderBook book;
        applyPreload(book, preloadedAsks);

        const auto start = Clock::now();
        for (const LimitOrder& order : crossingOrders) {
            book.getMatching().checkLimitOrder(order);
        }
        const auto end = Clock::now();

        const std::chrono::duration<double> d = end - start;
        const double seconds = d.count();
        const double opsPerSec = static_cast<double>(crossingOrders.size()) / seconds;
        secs.push_back(seconds);
        mops.push_back(opsPerSec / 1e6);

        requireBenchmark(validateBook(book), "matching trial produced invalid book state");
        requireBenchmark(book.getPrices().isBuyStoreEmpty(), "matching trial unexpectedly left buy resting orders");
        requireBenchmark(!book.getPrices().isSellStoreEmpty(), "matching trial fully depleted asks unexpectedly");
        consumeBookState(book);
    }

    ThroughputStats s;
    s.scenario = "Matching (crossing limits)";
    s.operations = crossingOrders.size();
    s.medianSeconds = medianOf(secs);
    s.medianMops = medianOf(mops);
    s.medianOpsPerSec = s.medianMops * 1e6;
    s.medianNsPerOp = (s.medianSeconds / static_cast<double>(crossingOrders.size())) * 1e9;
    s.minMops = *std::min_element(mops.begin(), mops.end());
    s.maxMops = *std::max_element(mops.begin(), mops.end());
    return s;
}

ThroughputStats runMixedThroughput(const Config& cfg, const MixedWorkload& w) {
    std::vector<double> secs;
    std::vector<double> mops;
    secs.reserve(cfg.trials);
    mops.reserve(cfg.trials);

    for (std::size_t t = 0; t < cfg.trials; ++t) {
        OrderBook book;
        applyPreload(book, w.preloadLiquidity);
        applyPreload(book, w.preloadCancelable);

        const auto start = Clock::now();
        for (const BenchOp& op : w.ops) {
            switch (op.type) {
                case OpType::PassiveLimit:
                    book.getMatching().checkLimitOrder(
                        LimitOrder{op.id, OrderType::LIMIT, op.side, op.qty, static_cast<double>(op.price)});
                    break;
                case OpType::Cancel:
                    book.getCancel().cancelOrder(op.id);
                    break;
                case OpType::CrossingLimit:
                    book.getMatching().checkLimitOrder(
                        LimitOrder{op.id, OrderType::LIMIT, op.side, op.qty, static_cast<double>(op.price)});
                    break;
                case OpType::Market:
                    book.getMatching().checkMarketOrder(
                        MarketOrder{op.id, OrderType::MARKET, op.side, op.qty});
                    break;
            }
        }
        const auto end = Clock::now();

        const std::chrono::duration<double> d = end - start;
        const double seconds = d.count();
        const double opsPerSec = static_cast<double>(w.ops.size()) / seconds;
        secs.push_back(seconds);
        mops.push_back(opsPerSec / 1e6);

        requireBenchmark(validateBook(book), "mixed trial produced invalid book state");
        consumeBookState(book);
    }

    ThroughputStats s;
    s.scenario = "Mixed workload";
    s.operations = w.ops.size();
    s.medianSeconds = medianOf(secs);
    s.medianMops = medianOf(mops);
    s.medianOpsPerSec = s.medianMops * 1e6;
    s.medianNsPerOp = (s.medianSeconds / static_cast<double>(w.ops.size())) * 1e9;
    s.minMops = *std::min_element(mops.begin(), mops.end());
    s.maxMops = *std::max_element(mops.begin(), mops.end());
    return s;
}

LatencyStats measureInsertionLatency(std::size_t count, std::uint32_t seed) {
    IdGenerator ids;
    std::vector<LimitOrder> orders = makePassiveNonCrossingOrders(count, ids);

    OrderBook book;
    std::vector<std::int64_t> durationsNs;
    durationsNs.reserve(orders.size());

    const auto totalStart = Clock::now();
    for (const LimitOrder& order : orders) {
        const auto start = Clock::now();
        book.getMatching().checkLimitOrder(order);
        const auto end = Clock::now();
        durationsNs.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
    }
    const auto totalEnd = Clock::now();

    requireBenchmark(validateBook(book), "insertion latency pass produced invalid book state");
    consumeBookState(book);

    std::sort(durationsNs.begin(), durationsNs.end());

    (void)seed;
    LatencyStats s;
    s.scenario = "Latency - insertion";
    s.operations = orders.size();
    s.totalSeconds = std::chrono::duration<double>(totalEnd - totalStart).count();
    s.p50Ns = percentileFromSorted(durationsNs, 0.50);
    s.p95Ns = percentileFromSorted(durationsNs, 0.95);
    s.p99Ns = percentileFromSorted(durationsNs, 0.99);
    return s;
}

LatencyStats measureCancellationLatency(std::size_t count, std::uint32_t seed) {
    IdGenerator ids;
    std::vector<LimitOrder> population = makeCancelPopulationOrders(count, ids);
    std::vector<int> cancelIds = makeCancellationIdsFromPopulation(population, seed);

    OrderBook book;
    applyPreload(book, population);

    std::vector<std::int64_t> durationsNs;
    durationsNs.reserve(cancelIds.size());

    const auto totalStart = Clock::now();
    for (int id : cancelIds) {
        const auto start = Clock::now();
        book.getCancel().cancelOrder(id);
        const auto end = Clock::now();
        durationsNs.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
    }
    const auto totalEnd = Clock::now();

    requireBenchmark(validateBook(book), "cancellation latency pass produced invalid book state");
    consumeBookState(book);

    std::sort(durationsNs.begin(), durationsNs.end());

    LatencyStats s;
    s.scenario = "Latency - cancellation";
    s.operations = cancelIds.size();
    s.totalSeconds = std::chrono::duration<double>(totalEnd - totalStart).count();
    s.p50Ns = percentileFromSorted(durationsNs, 0.50);
    s.p95Ns = percentileFromSorted(durationsNs, 0.95);
    s.p99Ns = percentileFromSorted(durationsNs, 0.99);
    return s;
}

LatencyStats measureMatchingLatency(std::size_t count) {
    IdGenerator ids;
    std::vector<LimitOrder> preloadedAsks = makeMatchingPreloadAsks(count, ids);
    std::vector<LimitOrder> crossingOrders = makeCrossingBuyLimits(count, ids);

    OrderBook book;
    applyPreload(book, preloadedAsks);

    std::vector<std::int64_t> durationsNs;
    durationsNs.reserve(crossingOrders.size());

    const auto totalStart = Clock::now();
    for (const LimitOrder& order : crossingOrders) {
        const auto start = Clock::now();
        book.getMatching().checkLimitOrder(order);
        const auto end = Clock::now();
        durationsNs.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
    }
    const auto totalEnd = Clock::now();

    requireBenchmark(validateBook(book), "matching latency pass produced invalid book state");
    consumeBookState(book);

    std::sort(durationsNs.begin(), durationsNs.end());

    LatencyStats s;
    s.scenario = "Latency - matching";
    s.operations = crossingOrders.size();
    s.totalSeconds = std::chrono::duration<double>(totalEnd - totalStart).count();
    s.p50Ns = percentileFromSorted(durationsNs, 0.50);
    s.p95Ns = percentileFromSorted(durationsNs, 0.95);
    s.p99Ns = percentileFromSorted(durationsNs, 0.99);
    return s;
}

double estimateNowOverheadNs(std::size_t iterations) {
    std::vector<std::int64_t> overhead;
    overhead.reserve(iterations);

    for (std::size_t i = 0; i < iterations; ++i) {
        const auto t0 = Clock::now();
        const auto t1 = Clock::now();
        overhead.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count());
    }

    return medianOf(overhead);
}

void printHeader() {
    std::cout << "\n"
              << "Scenario | Operations | Median Time (s) | M operations/s | Average ns/op | Min M ops/s | Max M ops/s | p50 ns | p95 ns | p99 ns\n"
              << "---------|------------|-----------------|----------------|---------------|-------------|-------------|--------|--------|-------\n";
}

void printThroughputRow(const ThroughputStats& s) {
    std::cout << s.scenario << " | "
              << s.operations << " | "
              << std::fixed << std::setprecision(6) << s.medianSeconds << " | "
              << std::fixed << std::setprecision(3) << s.medianMops << " | "
              << std::fixed << std::setprecision(3) << s.medianNsPerOp << " | "
              << std::fixed << std::setprecision(3) << s.minMops << " | "
              << std::fixed << std::setprecision(3) << s.maxMops << " | "
              << "- | - | -\n";
}

void printLatencyRow(const LatencyStats& s) {
    std::cout << s.scenario << " | "
              << s.operations << " | "
              << std::fixed << std::setprecision(6) << s.totalSeconds << " | "
              << "- | - | - | - | "
              << std::fixed << std::setprecision(1) << s.p50Ns << " | "
              << std::fixed << std::setprecision(1) << s.p95Ns << " | "
              << std::fixed << std::setprecision(1) << s.p99Ns << "\n";
}

bool parseSizeArg(const std::string& arg, const std::string& key, std::size_t& out) {
    if (arg.rfind(key, 0) != 0) {
        return false;
    }
    out = static_cast<std::size_t>(std::stoull(arg.substr(key.size())));
    return true;
}

bool parseU32Arg(const std::string& arg, const std::string& key, std::uint32_t& out) {
    if (arg.rfind(key, 0) != 0) {
        return false;
    }
    out = static_cast<std::uint32_t>(std::stoul(arg.substr(key.size())));
    return true;
}

void parseArgs(int argc, char** argv, Config& cfg) {
    for (int i = 1; i < argc; ++i) {
        const std::string arg(argv[i]);

        if (arg == "--help") {
            std::cout << "Usage: order_book_benchmark.exe [options]\n"
                      << "  --trials=<n>\n"
                      << "  --seed=<u32>\n"
                      << "  --warmup=<n>\n"
                      << "  --passive-ops=<n>\n"
                      << "  --cancel-ops=<n>\n"
                      << "  --matching-ops=<n>\n"
                      << "  --mixed-ops=<n>\n"
                      << "  --latency-ops=<n>\n";
            std::exit(0);
        }

        if (parseSizeArg(arg, "--trials=", cfg.trials)) {
            continue;
        }
        if (parseU32Arg(arg, "--seed=", cfg.seed)) {
            continue;
        }
        if (parseSizeArg(arg, "--warmup=", cfg.warmupOps)) {
            continue;
        }
        if (parseSizeArg(arg, "--passive-ops=", cfg.passiveOps)) {
            continue;
        }
        if (parseSizeArg(arg, "--cancel-ops=", cfg.cancelOps)) {
            continue;
        }
        if (parseSizeArg(arg, "--matching-ops=", cfg.matchingOps)) {
            continue;
        }
        if (parseSizeArg(arg, "--mixed-ops=", cfg.mixedOps)) {
            continue;
        }
        if (parseSizeArg(arg, "--latency-ops=", cfg.latencyOps)) {
            continue;
        }
    }

    if (cfg.warmupOps < 100000U) {
        cfg.warmupOps = 100000U;
    }
}

} // namespace

int main(int argc, char** argv) {
    Config cfg;
    parseArgs(argc, argv, cfg);

    std::cout << "Order book benchmark (single-threaded)\n";
    std::cout << "Config: trials=" << cfg.trials
              << ", seed=" << cfg.seed
              << ", warmup=" << cfg.warmupOps
              << ", passiveOps=" << cfg.passiveOps
              << ", cancelOps=" << cfg.cancelOps
              << ", matchingOps=" << cfg.matchingOps
              << ", mixedOps=" << cfg.mixedOps
              << ", latencyOps=" << cfg.latencyOps
              << "\n";

    IdGenerator idGen;

    const std::vector<LimitOrder> passiveOrders = makePassiveNonCrossingOrders(cfg.passiveOps, idGen);
    const std::vector<LimitOrder> cancelPopulation = makeCancelPopulationOrders(cfg.cancelOps, idGen);
    const std::vector<int> cancelIds = makeCancellationIdsFromPopulation(cancelPopulation, cfg.seed + 1U);
    const std::vector<LimitOrder> matchingPreloadAsks = makeMatchingPreloadAsks(cfg.matchingOps, idGen);
    const std::vector<LimitOrder> crossingBuyOrders = makeCrossingBuyLimits(cfg.matchingOps, idGen);
    const MixedWorkload mixed = makeMixedWorkload(cfg, idGen);

    warmupPassive(passiveOrders, cfg.warmupOps);
    warmupCancel(cancelPopulation, cancelIds, cfg.warmupOps);
    warmupMatching(matchingPreloadAsks, crossingBuyOrders, cfg.warmupOps);
    warmupMixed(mixed, cfg.warmupOps);

    const ThroughputStats passive = runPassiveThroughput(cfg, passiveOrders);
    const ThroughputStats cancel = runCancellationThroughput(cfg, cancelPopulation, cancelIds);
    const ThroughputStats matching = runMatchingThroughput(cfg, matchingPreloadAsks, crossingBuyOrders);
    const ThroughputStats mixedStats = runMixedThroughput(cfg, mixed);

    const double nowOverheadNs = estimateNowOverheadNs(500000U);

    const LatencyStats insertionLat = measureInsertionLatency(cfg.latencyOps, cfg.seed + 2U);
    const LatencyStats cancellationLat = measureCancellationLatency(cfg.latencyOps, cfg.seed + 3U);
    const LatencyStats matchingLat = measureMatchingLatency(cfg.latencyOps);

    printHeader();
    printThroughputRow(passive);
    printThroughputRow(cancel);
    printThroughputRow(matching);
    printThroughputRow(mixedStats);
    printLatencyRow(insertionLat);
    printLatencyRow(cancellationLat);
    printLatencyRow(matchingLat);

    std::cout << "\nsteady_clock::now() median overhead (ns): "
              << std::fixed << std::setprecision(1) << nowOverheadNs << "\n";
    std::cout << "Latency figures above include clock-measurement overhead and are not adjusted.\n";
    std::cout << "Post-state checks and invariants passed. Observable sink: " << gSink << "\n";

    return 0;
}
