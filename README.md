# order-book

# Required Downloads:
- Download the CLI11.hpp Latest release and put the file into the cli folder

# Benchmark Suite

Single-threaded benchmark executable:
- Source: benchmarks/order_book_benchmark.cpp
- Build script: benchmarks/build_benchmark.bat

## Build (Release)

Run from repository root:

```bat
benchmarks\build_benchmark.bat
```

Compiler flags used:
- -O3 -DNDEBUG -std=c++17 -Wall -Wextra

## Run

```bat
benchmarks\order_book_benchmark.exe
```

Optional arguments:
- --trials=<n>
- --seed=<u32>
- --warmup=<n>
- --passive-ops=<n>
- --cancel-ops=<n>
- --matching-ops=<n>
- --mixed-ops=<n>
- --latency-ops=<n>

Example:

```bat
benchmarks\order_book_benchmark.exe --trials=10 --mixed-ops=5000000 --seed=123456789 --warmup=100000
```

## Reporting Notes

- The benchmark is single-threaded.
- Publish CPU details alongside results (model, base/boost clocks, core count, SMT on/off, power mode).
- Publish OS details (Windows version/build, scheduler settings, background load conditions).
- For CV use, the primary metric should be median single-threaded throughput from the Mixed workload scenario.
- p99 latency should come only from the separate latency pass and is not interchangeable with bulk average ns/op.