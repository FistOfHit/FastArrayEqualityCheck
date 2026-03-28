# FastArrayEqualityCheck

`FastArrayEqualityCheck` is the repository for `FAE`, a C++20 library for
probabilistic array fingerprinting.

The library computes deterministic fingerprints for integral arrays and exposes
utilities for comparing those fingerprints. `FAE` does not claim proof of array
equality or inequality. Its public API only reports whether two produced
fingerprints match or differ under the configured algorithm.

## Status

This repository currently provides:

- A primary high-level API for fingerprinting and comparing integral arrays.
- Advanced low-level primitives for Szudzik pairing and 64-bit pairwise
  folding.
- CMake packaging, unit tests, examples, generated API docs, and CI support.
- Benchmark and collision-study executables for empirical evaluation.

## Guarantees

`FAE` is explicitly probabilistic.

- `fingerprints_match` means two arrays produced the same fingerprint under the
  configured algorithm.
- `fingerprints_differ` means two arrays produced different fingerprints under
  the configured algorithm.
- Neither result is described as a proof about the original arrays.

The library is designed to be deterministic and easy to reason about, not to
replace cryptographic or exact equality workflows.

## API Stability Notes

- `Fingerprint` is a stable source-level API type for v1.
- `Fingerprint` is not a stable serialized wire format in v1.
- Raw `Fingerprint` bytes should not be persisted or exchanged across versions,
  platforms, or ABIs.
- Fingerprints are intended to be compared only when they were produced from
  the same element type, seed, and algorithm configuration.
- The default algorithm in v1 is `fae::FingerprintAlgorithm::hash_combine`.
- `fae::FingerprintAlgorithm::legacy_fold` remains available as an opt-in
  comparison path.
- `fae::FingerprintAlgorithm::cantor_mix` is available as a public opt-in
  alternative for side-by-side evaluation without changing the default.
- `fae::FingerprintAlgorithm::tuple_mix` is available as a public opt-in
  alternative for side-by-side evaluation without changing the default.

## Quick Start

```cpp
#include <fae/fae.h>
#include <cstdint>
#include <span>
#include <vector>

int main() {
    const std::vector<std::int32_t> lhs {1, 2, 3, 4};
    const std::vector<std::int32_t> rhs {1, 2, 3, 4};
    const fae::FingerprintConfig config {.seed = 0};

    const fae::Fingerprint fingerprint = fae::fingerprint(std::span(lhs), config);
    const fae::FingerprintComparison comparison =
        fae::compare_fingerprints(std::span(lhs), std::span(rhs), config);

    return fingerprint.length == 4 &&
                   comparison == fae::FingerprintComparison::fingerprints_match
               ? 0
               : 1;
}
```

## Build

### Requirements

- CMake 3.21 or newer
- A C++20 compiler

### Configure, build, and test

```bash
cmake -S . -B build -DFAE_BUILD_TESTS=ON -DFAE_BUILD_EXAMPLES=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

### Build API docs

```bash
cmake -S . -B build -DFAE_BUILD_DOCS=ON
cmake --build build --target docs
```

### Build and run the measurement tools

```bash
cmake -S . -B build -DFAE_BUILD_BENCHMARKS=ON
cmake --build build
./build/fae_benchmark
./build/fae_collision_study
```

For a reduced CI-style pass:

```bash
./build/fae_benchmark --profile=smoke
./build/fae_collision_study --profile=smoke
```

For a larger local campaign:

```bash
./build/fae_benchmark --profile=deep
./build/fae_collision_study --profile=deep --seed=123456789
```

To scaffold a new versioned results snapshot from fresh measurements:

```bash
scripts/create_results_snapshot.sh deep-baseline --profile=deep
```

To promote a checked-in snapshot as the current baseline:

```bash
bash scripts/promote_results_snapshot.sh docs/results/2026-03-27-deep-baseline.md
```

### Install

```bash
cmake -S . -B build
cmake --build build
cmake --install build --prefix ./install
```

The package exports a CMake target that downstream projects can consume with
`find_package(fae CONFIG REQUIRED)`.

## Repository Layout

- `include/fae/`: public headers
- `src/`: library implementation
- `tests/`: unit tests and package smoke test consumer
- `examples/`: small usage examples
- `docs/`: narrative documentation
- `cmake/`: packaging and test helper scripts
- `benchmarks/`: empirical throughput and collision-study tools
- `scripts/`: local project helpers

## Documentation

- Agent guide: `AGENTS.md`
- API overview: `docs/api.md`
- AI operator guide: `docs/ai-agent-guide.md`
- Architecture notes: `docs/architecture.md`
- Baseline results: `docs/benchmark-baseline.md`
- Results archive: `docs/results/index.md`
- Empirical evaluation: `docs/empirical-evaluation.md`
- Format and stability notes: `docs/fingerprint-format.md`
- Probabilistic semantics: `docs/probabilistic-model.md`
- Project backlog: `TODO.md`
- Contributing guide: `CONTRIBUTING.md`

## Future Pairing And Folding Work

`FAE` currently implements Szudzik pairing as its exact advanced pairing
primitive. The docs also call out candidate future additions:

- Cantor pairing
- Rosenberg-Strong pairing
- Morton / Z-order interleaving
- Additional ordered hash-combine variants

These remain documented extension points rather than implemented public APIs.

## License

This project is licensed under the MIT License. See `LICENSE` for details.
