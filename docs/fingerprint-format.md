# Fingerprint Format And Stability

`fae::Fingerprint` is a public API type, but v1 does not define it as a stable
serialized format.

## What is stable in v1

- The source-level fields: `low`, `high`, and `length`
- Deterministic production for the same input, element type, order, and seed
- Equality comparison of `Fingerprint` values within the same build/runtime

## What is not stable in v1

- Raw byte layout across ABIs or platforms
- Persisted binary compatibility across releases
- Any promise that future algorithm revisions will preserve the same
  `low`/`high` values

## Practical guidance

- Use `Fingerprint` as an in-memory comparison value.
- Do not write raw `Fingerprint` bytes to disk as a long-term format.
- Do not exchange raw `Fingerprint` bytes between processes that may differ by
  compiler, platform, or library version.
- If a future release needs stable serialization, it should introduce an
  explicit versioned wire format instead of treating this struct as one.
