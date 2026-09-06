# Benchmarks

## Environment

- CPU: 12th Gen Intel(R) Core(TM) i5-12500H
- RAM: 32GB DDR4 3200 MT/s
- Operating system: Windows 11
- Compiler: GCC-6.3.0-1
- Build type: Release
- Compiler flags: `-O3 -DNDEBUG`
- Threads: 1

## Configuration

- Trials: 10
- Random seed: `123456789`
- Warm-up operations: 100,000
- Passive insertion operations: 1,000,000
- Cancellation operations: 1,000,000
- Matching operations: 1,000,000
- Mixed-workload operations: 5,000,000

## Results

| Scenario | Median time | Throughput | Average time |
|---|---:|---:|---:|
| Passive insertion | 0.129014 s | 7.751M ops/s | 129.013 ns/op |
| Cancellation | 0.319238 s | 3.132M ops/s | 319.238 ns/op |
| Crossing-limit matching | 0.019001 s | 52.629M ops/s | 19.001 ns/op |
| Mixed workload | 0.657223 s | **7.608M ops/s** | **131.445 ns/op** |

## Mixed Workload

The mixed benchmark consists of:

- 40% passive limit-order insertions
- 20% cancellations of active orders
- 20% crossing limit orders
- 20% market orders

Commands are generated before timing using a fixed random seed. Order IDs are
unique, cancellations reference active orders, and the benchmark maintains
bounded book depth.

## Methodology

All benchmarks are single-threaded. Commands and random values are generated
before the timed region. Each scenario performs an untimed warm-up followed by
10 measured trials using a fresh, equivalently configured order book.

Results report the median trial. Benchmark loops contain no logging, file I/O
or random-number generation. Post-benchmark invariants verify the resulting
book state and expose an observable value to prevent dead-code elimination.

## Latency Measurement

Individual-operation latency is not currently reported because the available
`std::chrono::steady_clock` resolution is insufficient for operations at this
timescale. The bulk `ns/op` values represent total elapsed time divided by the
number of operations and must not be interpreted as p99 latency.
