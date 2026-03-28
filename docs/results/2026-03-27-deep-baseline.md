# 2026-03-27 Deep Baseline

This file records the first deeper local comparison baseline after adding
algorithm-to-algorithm measurement support, measurement profiles, and the
versioned results archive.

## Environment

- Date: 2026-03-27
- Platform: macOS arm64
- Compiler family used by the build: AppleClang 21
- Build flow:

```bash
cmake --build build --target fae_benchmark fae_collision_study
./build/fae_benchmark --profile=deep
./build/fae_collision_study --profile=deep
```

## High-level findings

- In the deeper collision study, all three compared algorithms again reported
  zero observed collisions.
- The deep collision study covered 3,240,000 total generated samples:
  - 3 algorithms
  - 3 element types
  - 3 array lengths
  - 3 input distributions
- `hash_combine` was materially faster than `legacy_fold` in the sampled
  throughput runs.
- `cantor_mix` also outperformed `legacy_fold`, but generally trailed
  `hash_combine`.

## Throughput excerpt

Selected `int32_t` fingerprint rows from the deep local baseline:

| algorithm | size | ns/op | MiB/s |
| --- | ---: | ---: | ---: |
| `legacy_fold` | 64 | 48890.22 | 4.99 |
| `hash_combine` | 64 | 3044.43 | 80.19 |
| `cantor_mix` | 64 | 4790.70 | 50.96 |
| `legacy_fold` | 1024 | 592214.39 | 6.60 |
| `hash_combine` | 1024 | 62412.97 | 62.59 |
| `cantor_mix` | 1024 | 91892.58 | 42.51 |
| `legacy_fold` | 16384 | 8776609.33 | 7.12 |
| `hash_combine` | 16384 | 851248.29 | 73.42 |
| `cantor_mix` | 16384 | 1514118.49 | 41.28 |
| `legacy_fold` | 262144 | 157762951.17 | 6.34 |
| `hash_combine` | 262144 | 14161421.88 | 70.61 |
| `cantor_mix` | 262144 | 27033418.61 | 36.99 |

These are local results, not portable guarantees. They are most useful as a
reference point for future runs on the same class of machine.

## Collision excerpt

Observed collision count in the deep sampled run:

- `legacy_fold`: 0
- `hash_combine`: 0
- `cantor_mix`: 0

This is an empirical observation, not a proof or formal bound.

## Interpretation

This deeper local baseline was strong enough to justify switching the v1
default to `hash_combine`. On this run, `hash_combine` dominates
`legacy_fold` on throughput while matching it on the sampled collision study,
and `cantor_mix` remains a slower middle ground.
