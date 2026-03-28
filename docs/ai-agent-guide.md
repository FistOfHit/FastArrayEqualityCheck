# AI Agent Guide

This guide is for AI agents that need to understand, use, modify, or evaluate
`FAE` without re-deriving the repository structure from scratch.

## Purpose

`FAE` is a C++20 library for deterministic probabilistic fingerprinting of
same-type integral arrays.

Its job is to produce compact fingerprints and compare those fingerprints. Its
job is not to prove array equality.

## Core Contract

The public API lives in `include/fae/fingerprint.h` and is re-exported by
`include/fae/fae.h`.

Primary types:

- `fae::Fingerprint`
- `fae::FingerprintConfig`
- `fae::FingerprintAlgorithm`
- `fae::FingerprintComparison`

Primary functions:

- `fae::fingerprint(std::span<const T>, const fae::FingerprintConfig&)`
- `fae::compare_fingerprints(std::span<const T>, std::span<const T>, const fae::FingerprintConfig&)`

Hard semantic rules:

- Inputs are same-type integral spans in v1.
- Fingerprints are deterministic for the same input, seed, and algorithm.
- `fingerprints_match` means the produced fingerprints matched.
- `fingerprints_differ` means the produced fingerprints differed.
- Neither result is a proof about the original arrays.
- `Fingerprint` is not a stable serialized wire format.

## Current Public Algorithms

- `hash_combine`
  Current default. Conservative public default for v1.
- `tuple_mix`
  Public opt-in alternative. Currently the strongest non-default public
  challenger in the latest promoted deep snapshot.
- `cantor_mix`
  Public opt-in alternative based on Cantor pairing over 16-bit fragments.
- `legacy_fold`
  Older public path kept for compatibility and comparison.

If you need the current evidence for which algorithm looks best, read
`docs/benchmark-baseline.md` first, then the linked current snapshot.

## Quick Usage

Minimal fingerprint example:

```cpp
#include <fae/fae.h>

#include <cstdint>
#include <span>
#include <vector>

int main() {
    const std::vector<std::int32_t> values {4, 8, 15, 16, 23, 42};
    const fae::Fingerprint fp = fae::fingerprint(std::span(values));
    return fp.length == values.size() ? 0 : 1;
}
```

Minimal comparison example:

```cpp
#include <fae/fae.h>

#include <cstdint>
#include <span>
#include <vector>

int main() {
    const std::vector<std::uint32_t> lhs {1, 2, 3, 4};
    const std::vector<std::uint32_t> rhs {1, 2, 3, 5};

    const fae::FingerprintComparison result =
        fae::compare_fingerprints(std::span(lhs), std::span(rhs));

    return result == fae::FingerprintComparison::fingerprints_differ ? 0 : 1;
}
```

Selecting a specific algorithm:

```cpp
const fae::FingerprintConfig config {
    .seed = 12345U,
    .algorithm = fae::FingerprintAlgorithm::tuple_mix
};
```

## How To Interpret Results

Good language:

- "The fingerprints matched under `hash_combine`."
- "The fingerprints differed under `tuple_mix`."
- "This is probabilistic, not a proof of equality."

Bad language:

- "These arrays are definitely equal."
- "This proves the arrays are different."
- "The fingerprint is a stable wire format."

## Build And Verify

Standard build/test flow:

```bash
cmake -S . -B build -DFAE_BUILD_TESTS=ON -DFAE_BUILD_EXAMPLES=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

Docs:

```bash
cmake -S . -B build -DFAE_BUILD_DOCS=ON
cmake --build build --target docs
```

Measurements:

```bash
cmake -S . -B build -DFAE_BUILD_BENCHMARKS=ON
cmake --build build --target fae_benchmark fae_collision_study
./build/fae_benchmark --profile=smoke
./build/fae_collision_study --profile=smoke
```

Deep local study:

```bash
./build/fae_benchmark --profile=deep
./build/fae_collision_study --profile=deep --seed=123456789
```

## Repo Map

- `include/fae/`
  Public headers. Start here for API truth.
- `src/`
  Library implementation.
- `examples/`
  Minimal usage programs.
- `tests/`
  Unit tests and package smoke test.
- `docs/`
  Narrative docs and measurement archive.
- `benchmarks/`
  Throughput and collision-study executables.
- `scripts/`
  Snapshot capture and promotion helpers.

## Where To Look For Truth

For public semantics:

1. `include/fae/fingerprint.h`
2. `docs/api.md`
3. `README.md`

For algorithm-decision context:

1. `docs/benchmark-baseline.md`
2. The linked current snapshot under `docs/results/`
3. `docs/empirical-evaluation.md`

For implementation details:

1. `src/fingerprint.cpp`
2. `tests/fingerprint_tests.cpp`

## Agent Task Cookbook

### If the user wants to use the library

- Point them to `#include <fae/fae.h>`.
- Show `FingerprintConfig` only if they need a non-default seed or algorithm.
- Remind them that results are probabilistic.

### If the user wants to evaluate algorithm choice

- Read `docs/benchmark-baseline.md`.
- Use the promoted current snapshot, not older archived snapshots, for the main
  recommendation.
- Treat older snapshots as historical context only.

### If the user wants to change public behavior

- Update tests first or together with code.
- Update `README.md`, `docs/api.md`, and any affected narrative docs.
- Rebuild docs after changes.

### If the user wants a new measurement baseline

- Run:

```bash
bash scripts/create_results_snapshot.sh <slug> --profile=deep
```

- Review and condense the generated markdown under `docs/results/`.
- Promote it if it materially changes the story:

```bash
bash scripts/promote_results_snapshot.sh docs/results/<date>-<slug>.md
```

### If you are tempted to rewrite old result snapshots

Do not do that unless the user explicitly asks. Historical snapshot files are
meant to preserve what the repo believed at the time of that run.

## Recommended Default Agent Workflow

1. Read `AGENTS.md`.
2. Read `README.md`.
3. Read `include/fae/fingerprint.h`.
4. Read `docs/benchmark-baseline.md` if the task touches algorithm choice.
5. Build or test only the smallest relevant surface first.

## Current Stop Point

As of the latest promoted baseline:

- `hash_combine` remains the default.
- `tuple_mix` has the strongest current performance case among non-default
  public algorithms.
- `cantor_mix` is public and measured, but is not the strongest default
  challenger in the latest promoted deep snapshot.
