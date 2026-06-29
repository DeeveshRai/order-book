# Order Book Performance Notes

| Price Range       | Time   |
| ----------------- | ------ |
| 1 price level     | 0.175s |
| 10 price levels   | 0.268s |
| 100 price levels  | 0.540s |
| 1000 price levels | 0.831s |

1 Price Level      - 0.174515s
10 Price Levels    - 0.268242s
100 Price Levels   - 0.540022s
1000 Price Levels  - 0.830629s - now Elapsed Time: 0.772857000s

Alternating Match  - 0.517022s
Deep Book          - 0.740722s

Cancellation (1M)  - 0.076362s

Matching (-O0)     - 1.270780s
Matching (-O3)     - 0.301290s



## Environment

Compiler Flags:

```bash
-O3 -march=native -std=c++17
```

---

## Benchmark 1 - Matching (Single Price Level)

### Workload

```text
1,000,000 LIMIT BUY 100 @ 10
1,000,000 LIMIT SELL 100 @ 10
```

### Result

```text
Elapsed Time: 0.30129s
```

### Throughput

```text
2,000,000 orders / 0.30129s

≈ 6.64 million orders/sec
≈ 150 ns per order
```

### Notes

* Best-case scenario
* Single price level
* Perfect branch predictability
* Minimal traversal
* Heavy matching workload

---

## Benchmark 2 - Matching (Random Price Levels)

### Workload

```text
1,000,000 LIMIT BUY orders
1,000,000 LIMIT SELL orders

Price range:
0 - 999
```

### Result

```text
Elapsed Time: 0.517022s
```

### Throughput

```text
2,000,000 orders / 0.517022s

≈ 3.87 million orders/sec
≈ 258 ns per order
```

### Notes

* More realistic workload
* Multiple price levels
* Less predictable branches
* Increased map operations
* Increased traversal complexity

---

## Benchmark 3 - Cancellation

### Workload

```text
Insert 1,000,000 BUY orders

Store all generated IDs

Cancel all 1,000,000 orders
```

### Result

```text
Elapsed Time: 0.076362s
```

### Throughput

```text
1,000,000 cancellations / 0.076362s

≈ 13.1 million cancellations/sec
≈ 76 ns per cancellation
```

### Notes

* Cancellation does not appear to be a major bottleneck
* Likely dominated by:

  * ID lookup
  * Node unlinking
  * Price-level maintenance

---

# Key Discoveries

## Compiler Optimisation

Using:

```bash
-O3 -march=native
```

improved performance significantly.

### Before

```text
2,000,000 matching orders

Elapsed Time: 1.27078s
≈ 1.57 million orders/sec
```

### After

```text
2,000,000 matching orders

Elapsed Time: 0.30129s
≈ 6.64 million orders/sec
```

### Improvement

```text
~4.2x speedup
```

---

# Current Conclusions

* Matching is currently the most expensive operation measured.
* Cancellation is not a significant bottleneck.
* Compiler optimisation provides substantial gains before any code changes.
* Benchmarking uncovered a real correctness bug in ID generation.
* Future optimisation work should focus on:

  * Matching traversal
  * Price-level maintenance
  * Cache locality
  * Data structure efficiency
  * Profiling hot paths before optimisation

---

# Next Steps

1. Build additional benchmark scenarios:

   * Insert-only workload
   * Deep book workload
   * Heavy cancellation workload
   * Mixed realistic market workload

2. Profile the application:

   * Identify hot functions
   * Measure cache behaviour
   * Measure branch prediction performance

3. Optimise only verified bottlenecks.

Rule:

```text
Measure -> Profile -> Optimise -> Measure Again
```
