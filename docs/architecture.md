# Architecture

## Design goals

- Deterministic probabilistic fingerprints for integral arrays
- Small public API
- Stable CMake package for downstream consumers
- Clear separation between public API, advanced primitives, and future ideas

## Current fingerprint pipeline

The high-level fingerprint path is intentionally not built only on exact
pairing. Exact pair encoders are useful, but they do not scale cleanly as the
sole primitive for a generic probabilistic array API.

The current flow is:

1. Normalize each integral element into a 64-bit value.
2. Mix each element with a deterministic seeded mixer and its position.
3. Fold the sequence into one 64-bit lane.
4. Repeat with a second independent lane.
5. Store both lanes plus the input length in `fae::Fingerprint`.

## Advanced primitives

- `Szudzik pairing` is the first exact pair encoder.
- `pairwise_fold64` is the probabilistic reduction primitive used to fold
  ordered 64-bit sequences.

## Future extension points

The docs reserve room for more advanced methods without exposing a pluggable
strategy API yet.

Candidate pair encoders and folders:

- Szudzik pairing
- Cantor pairing
- Rosenberg-Strong pairing
- Morton / Z-order interleaving
- Additional ordered hash-combine variants

The public API now exposes `legacy_fold`, `hash_combine`, `cantor_mix`, and
`tuple_mix`. The current benchmark harness compares those production code paths
side by side so algorithm changes can be measured before the default changes.

Future additions should be judged on collision behavior, performance,
implementation clarity, and how well they preserve ordered-sequence semantics.
