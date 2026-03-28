# Benchmark Baseline

This page is the stable entry point for checked-in measurement history.

The repository now stores concrete benchmark snapshots under `docs/results/`
instead of rewriting one mutable baseline file forever. That makes it easier
to compare algorithm changes over time and to see which measurement snapshot
was used to justify a public API decision.

## Current baseline

<!-- BEGIN CURRENT_BASELINE -->
- [2026-03-27 Public Cantor Mix API Deep](results/2026-03-27-public-cantor-mix-api-deep.md)
<!-- END CURRENT_BASELINE -->

## Why this exists

- Keep one stable documentation entry point for benchmark history.
- Preserve the exact snapshot that justified a default-algorithm decision.
- Make future algorithm changes easier to compare without overwriting older
  evidence.

## How to use it

- Treat the linked snapshot as the current checked-in reference.
- Add a new file under `docs/results/` when a later run materially changes the
  algorithm story.
- Update this page and `docs/results/index.md` to point at the new current
  baseline.
