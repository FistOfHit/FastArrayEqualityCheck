# API Guide

`FAE` exposes one primary high-level API and a smaller advanced API.

## Primary API

Include:

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

The primary API accepts same-type integral spans in v1 and produces
deterministic results for a given seed.

Important usage rules:

- Compare fingerprints only when they were produced from the same element type.
- Compare fingerprints only when they were produced with the same seed.
- Compare fingerprints only when they were produced with the same algorithm.
- Treat `fae::Fingerprint` as an in-memory API type, not a serialized format.

`fae::FingerprintConfig` currently supports:

- `seed`
- `algorithm`

Available public algorithms:

- `fae::FingerprintAlgorithm::legacy_fold`
- `fae::FingerprintAlgorithm::hash_combine`
- `fae::FingerprintAlgorithm::cantor_mix`
- `fae::FingerprintAlgorithm::tuple_mix`

`hash_combine` is the default in v1. `legacy_fold` remains available as an
opt-in comparison path, and both `cantor_mix` and `tuple_mix` are available as
public opt-in alternatives.

## Advanced API

Advanced primitives are public but intentionally secondary:

- `fae::pairing::szudzik_pair`
- `fae::reduction::pairwise_fold64`

These functions are useful for experiments, diagnostics, and future extension
work. They are not the full public story of the library.
