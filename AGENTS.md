# Agent Guide

This repository contains `FAE`, a C++20 library for probabilistic array
fingerprinting.

If you are an AI agent operating in this repo, start here.

## What This Repo Is

- A small installable C++ library for deterministic fingerprint generation over
  same-type integral arrays.
- A comparison utility that reports whether two produced fingerprints match or
  differ.
- A measured, documented project with tests, examples, CMake packaging,
  generated docs, and benchmark/collision-study tools.

## What This Repo Is Not

- Not an exact equality engine.
- Not a cryptographic hash library.
- Not a generic container comparison framework.
- Not a stable serialized fingerprint format.

Do not describe `fingerprints_match` as proof that two arrays are equal.
Do not describe `fingerprints_differ` as proof about the source arrays beyond
the produced fingerprints differing.

## Fastest Entry Points

- Repo overview: `README.md`
- Public API contract: `include/fae/fingerprint.h`
- Narrative API guide: `docs/api.md`
- Current measurement baseline: `docs/benchmark-baseline.md`
- Versioned measurement history: `docs/results/index.md`
- Detailed agent/operator guide: `docs/ai-agent-guide.md`

## Public API In One Screen

Header:

```cpp
#include <fae/fae.h>
```

Primary types:

- `fae::Fingerprint`
- `fae::FingerprintConfig`
- `fae::FingerprintAlgorithm`
- `fae::FingerprintComparison`

Primary functions:

- `fae::fingerprint(std::span<const T>, const fae::FingerprintConfig&)`
- `fae::compare_fingerprints(std::span<const T>, std::span<const T>, const fae::FingerprintConfig&)`

Current public algorithms:

- `fae::FingerprintAlgorithm::hash_combine` (default)
- `fae::FingerprintAlgorithm::tuple_mix`
- `fae::FingerprintAlgorithm::cantor_mix`
- `fae::FingerprintAlgorithm::legacy_fold`

## Behavior Rules You Must Preserve

- Inputs are same-type integral spans in v1.
- Results are deterministic for the same input, seed, and algorithm.
- Fingerprints are type-tagged; cross-type comparisons are not the intended
  use.
- `Fingerprint` is an in-memory API type, not a wire format.
- Public docs and names must stay probabilistic and non-definitive.

## Most Common Commands

Configure, build, and test:

```bash
cmake -S . -B build -DFAE_BUILD_TESTS=ON -DFAE_BUILD_EXAMPLES=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

Build docs:

```bash
cmake -S . -B build -DFAE_BUILD_DOCS=ON
cmake --build build --target docs
```

Run measurement smoke checks:

```bash
cmake -S . -B build -DFAE_BUILD_BENCHMARKS=ON
cmake --build build --target fae_benchmark fae_collision_study
./build/fae_benchmark --profile=smoke
./build/fae_collision_study --profile=smoke
```

## Current Decision State

- `hash_combine` is still the public default.
- `tuple_mix` is currently the strongest non-default public challenger in the
  latest promoted deep snapshot.
- `cantor_mix` is public and measured, but it is not currently the leading
  challenger based on the latest promoted baseline.

When discussing performance or algorithm decisions, prefer the current promoted
baseline under `docs/benchmark-baseline.md` instead of older archived
snapshots.

## When Editing This Repo

- Update tests when behavior changes.
- Update public docs when public API or decision guidance changes.
- Treat older `docs/results/*.md` files as historical records unless the user
  explicitly asks to rewrite history.
- If a new measurement run materially changes the story, add a new snapshot and
  promote it instead of rewriting old ones.

## If You Need More Context

Read `docs/ai-agent-guide.md`.
