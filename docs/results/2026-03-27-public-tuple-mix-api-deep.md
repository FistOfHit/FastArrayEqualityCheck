# 2026-03-27 Public Tuple Mix API Deep

## Environment

- Date: 2026-03-27
- Profile: deep
- Platform: Darwin 25.3.0 arm64
- Compiler family used by the build: Apple clang version 21.0.0 (clang-2100.0.123.102)
- Build directory: `build`
- Capture script: `scripts/create_results_snapshot.sh`

## Build flow

```bash
cmake --build build --target fae_benchmark fae_collision_study
./build/fae_benchmark --profile=deep
./build/fae_collision_study --profile=deep
```

## Raw artifacts

- Benchmark output: `docs/results/artifacts/2026-03-27-public-tuple-mix-api-deep-benchmark.txt`
- Collision output: `docs/results/artifacts/2026-03-27-public-tuple-mix-api-deep-collision.txt`

## High-level findings

- In the deeper collision study, all four compared algorithms again reported
  zero observed collisions.
- This updated deep collision study covered 7,200,000 total generated samples:
  - 4 algorithms
  - 3 element types
  - 3 array lengths
  - 5 input distributions
- `cantor_mix` was the fastest sampled fingerprint algorithm on all 12 checked
  size and type combinations in this run, which makes it the current
  benchmark-local throughput leader.
- Public `tuple_mix` remained competitive and beat `hash_combine` on 8 of 12
  sampled fingerprint rows, but it no longer leads the full comparison set and
  still does not justify a default change by itself.

## Throughput excerpt

Selected fingerprint rows from the deep local comparison:

| type | algorithm | size | ns/op | MiB/s |
| --- | --- | ---: | ---: | ---: |
| `int32_t` | `hash_combine` | 64 | 4949.79 | 49.32 |
| `int32_t` | `cantor_mix` | 64 | 2594.55 | 94.10 |
| `int32_t` | `tuple_mix` | 64 | 3726.81 | 65.51 |
| `int32_t` | `hash_combine` | 262144 | 25696011.06 | 38.92 |
| `int32_t` | `cantor_mix` | 262144 | 12144188.14 | 82.34 |
| `int32_t` | `tuple_mix` | 262144 | 15981583.98 | 62.57 |
| `uint64_t` | `hash_combine` | 64 | 4097.83 | 119.16 |
| `uint64_t` | `cantor_mix` | 64 | 2428.75 | 201.04 |
| `uint64_t` | `tuple_mix` | 64 | 9414.25 | 51.87 |
| `uint64_t` | `hash_combine` | 262144 | 17111659.52 | 116.88 |
| `uint64_t` | `cantor_mix` | 262144 | 10869348.97 | 184.00 |
| `uint64_t` | `tuple_mix` | 262144 | 16470561.20 | 121.43 |
| `uint8_t` | `hash_combine` | 64 | 4340.43 | 14.06 |
| `uint8_t` | `cantor_mix` | 64 | 2355.38 | 25.91 |
| `uint8_t` | `tuple_mix` | 64 | 3913.38 | 15.60 |
| `uint8_t` | `hash_combine` | 262144 | 16788879.56 | 14.89 |
| `uint8_t` | `cantor_mix` | 262144 | 11051995.28 | 22.62 |
| `uint8_t` | `tuple_mix` | 262144 | 15223336.91 | 16.42 |

On the full sampled fingerprint set, `tuple_mix` beat `hash_combine` on 8 of
12 size and type combinations. The sampled `hash_combine` wins in this run
were the `uint64_t` size-64, size-1024, and size-16384 rows plus the `uint8_t`
size-16384 row. `cantor_mix` was faster than both public algorithms on every
sampled fingerprint row in this artifact.

## Collision excerpt

Observed collision count in the deep sampled run:

- `legacy_fold`: 0
- `hash_combine`: 0
- `cantor_mix`: 0
- `tuple_mix`: 0

This run covered five input distributions:

- `uniform_full_range`
- `low_range`
- `sparse_boolean`
- `monotonic_step`
- `paired_low_range`

The structured `monotonic_step` and `paired_low_range` workloads also reported
zero observed collisions for every compared algorithm in this run. This is an
empirical observation, not a proof or formal bound.

## Interpretation

This snapshot replaces the earlier tuple-mix comparison as the more accurate
baseline for the current codebase. It reflects the public `tuple_mix` API path
and the expanded five-distribution collision harness.

It does not justify changing the default away from `hash_combine`. Public
`tuple_mix` still looks viable as an opt-in algorithm, but it no longer
dominates the sampled throughput picture. The more important change is that
`cantor_mix` is now the strongest benchmark-local throughput candidate and
deserves to replace `tuple_mix` as the main experimental challenger for future
algorithm work.
