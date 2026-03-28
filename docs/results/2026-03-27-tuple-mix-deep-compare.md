# 2026-03-27 Tuple Mix Deep Compare

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

- Benchmark output: `docs/results/artifacts/2026-03-27-tuple-mix-deep-compare-benchmark.txt`
- Collision output: `docs/results/artifacts/2026-03-27-tuple-mix-deep-compare-collision.txt`

## High-level findings

- In the deeper collision study, all four compared algorithms again reported
  zero observed collisions.
- The deep collision study covered 4,320,000 total generated samples:
  - 4 algorithms
  - 3 element types
  - 3 array lengths
  - 3 input distributions
- `tuple_mix` was the fastest sampled fingerprint algorithm on all checked
  `int32_t` sizes and on most sampled `uint64_t` and `uint8_t` fingerprint
  rows.
- `hash_combine` remained faster on a small number of sampled rows, so this run
  strengthens the case for `tuple_mix` as the leading experimental candidate
  but does not, by itself, justify changing the public default.

## Throughput excerpt

Selected `int32_t` fingerprint rows from the deep local comparison:

| algorithm | size | ns/op | MiB/s |
| --- | ---: | ---: | ---: |
| `legacy_fold` | 64 | 23346.39 | 10.46 |
| `hash_combine` | 64 | 1617.19 | 150.97 |
| `cantor_mix` | 64 | 3184.76 | 76.66 |
| `tuple_mix` | 64 | 1508.15 | 161.88 |
| `legacy_fold` | 1024 | 329679.27 | 11.85 |
| `hash_combine` | 1024 | 26852.36 | 145.47 |
| `cantor_mix` | 1024 | 52743.14 | 74.06 |
| `tuple_mix` | 1024 | 19975.73 | 195.55 |
| `legacy_fold` | 16384 | 5112397.58 | 12.23 |
| `hash_combine` | 16384 | 506259.24 | 123.45 |
| `cantor_mix` | 16384 | 792110.80 | 78.90 |
| `tuple_mix` | 16384 | 394183.35 | 158.56 |
| `legacy_fold` | 262144 | 86833078.77 | 11.52 |
| `hash_combine` | 262144 | 8600743.48 | 116.27 |
| `cantor_mix` | 262144 | 11151822.27 | 89.67 |
| `tuple_mix` | 262144 | 5729598.31 | 174.53 |

On the full sampled fingerprint set, `tuple_mix` beat `hash_combine` on 10 of
12 size and type combinations, but not all of them. The known `hash_combine`
wins in this run were the `uint64_t` size-64 fingerprint row and the `uint8_t`
size-16384 fingerprint row. That matters because the public default should not
change casually on a single local run.

## Collision excerpt

Observed collision count in the deep sampled run:

- `legacy_fold`: 0
- `hash_combine`: 0
- `cantor_mix`: 0
- `tuple_mix`: 0

This is an empirical observation, not a proof or formal bound.

## Interpretation

This run materially changes the experimental algorithm story. `tuple_mix` is
now the strongest benchmark-local candidate in the repository and deserves to
replace `cantor_mix` as the main challenger to `hash_combine`.

It does not yet force a public default change. The data is still one local
deep-profile run, and `hash_combine` remained faster on a small number of
sampled fingerprint rows. The conservative next move is to keep
`hash_combine` as the public default, promote this snapshot as the current
baseline, and use it to drive a more targeted follow-up study.
