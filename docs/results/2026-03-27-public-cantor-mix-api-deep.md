# 2026-03-27 Public Cantor Mix API Deep

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

- Benchmark output: `docs/results/artifacts/2026-03-27-public-cantor-mix-api-deep-benchmark.txt`
- Collision output: `docs/results/artifacts/2026-03-27-public-cantor-mix-api-deep-collision.txt`

## High-level findings

- In the deeper collision study, all four public algorithms again reported zero
  observed collisions.
- This updated deep collision study covered 7,200,000 total generated samples:
  - 4 algorithms
  - 3 element types
  - 3 array lengths
  - 5 input distributions
- Public `tuple_mix` was the strongest sampled public throughput candidate in
  this run. It beat `hash_combine` on 11 of 12 sampled fingerprint rows.
- Public `cantor_mix` did not become the leading challenger after promotion. In
  this run it only beat `hash_combine` on one sampled fingerprint row.
- The current-code measurement story has therefore swung back toward
  `tuple_mix`, not `cantor_mix`, as the main non-default algorithm to evaluate
  against `hash_combine`.

## Throughput excerpt

Selected fingerprint rows from the deep local comparison:

| type | algorithm | size | ns/op | MiB/s |
| --- | --- | ---: | ---: | ---: |
| `int32_t` | `hash_combine` | 64 | 4713.94 | 51.79 |
| `int32_t` | `cantor_mix` | 64 | 5982.30 | 40.81 |
| `int32_t` | `tuple_mix` | 64 | 4263.50 | 57.26 |
| `int32_t` | `hash_combine` | 262144 | 18890881.52 | 52.94 |
| `int32_t` | `cantor_mix` | 262144 | 22960324.88 | 43.55 |
| `int32_t` | `tuple_mix` | 262144 | 17829615.23 | 56.09 |
| `uint64_t` | `hash_combine` | 64 | 4463.33 | 109.40 |
| `uint64_t` | `cantor_mix` | 64 | 5805.61 | 84.11 |
| `uint64_t` | `tuple_mix` | 64 | 4435.09 | 110.10 |
| `uint64_t` | `hash_combine` | 1024 | 71108.01 | 109.87 |
| `uint64_t` | `cantor_mix` | 1024 | 101552.04 | 76.93 |
| `uint64_t` | `tuple_mix` | 1024 | 71292.92 | 109.58 |
| `uint8_t` | `hash_combine` | 64 | 4539.00 | 13.45 |
| `uint8_t` | `cantor_mix` | 64 | 5834.54 | 10.46 |
| `uint8_t` | `tuple_mix` | 64 | 3865.58 | 15.79 |
| `uint8_t` | `hash_combine` | 262144 | 23388561.69 | 10.69 |
| `uint8_t` | `cantor_mix` | 262144 | 22761271.00 | 10.98 |
| `uint8_t` | `tuple_mix` | 262144 | 13476779.46 | 18.55 |

On the full sampled fingerprint set, `tuple_mix` beat `hash_combine` on 11 of
12 size and type combinations. The only sampled `hash_combine` win in this run
was the `uint64_t` size-1024 fingerprint row, and that margin was small.

`cantor_mix` beat `hash_combine` on only one sampled fingerprint row in this
artifact: `uint8_t` size-262144. That means promoting `cantor_mix` into the
public API did not turn it into the leading default challenger under this
current-code deep run.

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

This snapshot supersedes the earlier promoted baseline for the current codebase.
It reflects both public opt-in algorithms, `cantor_mix` and `tuple_mix`, and
the expanded five-distribution collision harness.

It does not force an immediate default change, but it makes the evaluation
point much sharper. On this run, `tuple_mix` materially outperformed
`hash_combine` on almost every sampled fingerprint row while matching it on the
sampled collision study. `cantor_mix` does not currently justify further
attention as the default challenger.

The clear next decision is whether `tuple_mix` should replace `hash_combine` as
the v1 default, or whether the project should stop here with `hash_combine` as
the conservative default despite the new evidence.
