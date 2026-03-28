# Results Archive

This directory holds versioned benchmark and collision-study summaries.

The archive exists so the repository can keep a stable historical record of
algorithm changes instead of rewriting a single rolling baseline file each
time the measurement story changes.

## Current snapshot

<!-- BEGIN CURRENT_SNAPSHOT -->
- [2026-03-27 Public Cantor Mix API Deep](2026-03-27-public-cantor-mix-api-deep.md)
<!-- END CURRENT_SNAPSHOT -->

## All snapshots

<!-- BEGIN SNAPSHOT_LIST -->
- [2026-03-27 Public Cantor Mix API Deep](2026-03-27-public-cantor-mix-api-deep.md)
- [2026-03-27 Public Tuple Mix API Deep](2026-03-27-public-tuple-mix-api-deep.md)
- [2026-03-27 Tuple Mix Deep Compare](2026-03-27-tuple-mix-deep-compare.md)
- [2026-03-27 Deep Baseline](2026-03-27-deep-baseline.md)
<!-- END SNAPSHOT_LIST -->

## Archive conventions

- Use one markdown file per meaningful measurement snapshot.
- Prefer filenames of the form `YYYY-MM-DD-<short-description>.md`.
- Record the environment, exact commands, and enough excerpted data to justify
  any algorithm decision tied to the run.
- Keep the checked-in snapshot concise; raw benchmark tables belong in local
  run artifacts, not in source control by default.
- Use `scripts/create_results_snapshot.sh <slug> --profile=<profile>` to
  capture fresh artifacts and scaffold a new snapshot from the template.
