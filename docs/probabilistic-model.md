# Probabilistic Model

`FAE` is probabilistic by design.

The library does not present any public result as proof of equality or
inequality of the original arrays. It reports only whether two generated
fingerprints match or differ under the configured algorithm.

## Why the API uses this language

The high-level API is intended to be honest about what it computes:

- `fingerprints_match`: the produced fingerprints are equal
- `fingerprints_differ`: the produced fingerprints are different

This avoids overclaiming what a non-cryptographic probabilistic fingerprint can
guarantee.

## What deterministic means here

The implementation is deterministic for a given seed, input order, and element
type:

- identical inputs produce identical fingerprints
- changing input order changes the produced fingerprint
- changing the element type changes the produced fingerprint
- a fixed seed produces stable outputs across runs

Deterministic output is about repeatability, not proof.
