# Contributing

## Local workflow

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

Run the measurement tools:

```bash
cmake -S . -B build -DFAE_BUILD_BENCHMARKS=ON
cmake --build build --target fae_benchmark fae_collision_study
./build/fae_benchmark --profile=smoke
./build/fae_collision_study --profile=smoke
```

Scaffold a new results snapshot:

```bash
scripts/create_results_snapshot.sh deep-baseline --profile=deep
```

Promote a checked-in snapshot:

```bash
bash scripts/promote_results_snapshot.sh docs/results/2026-03-27-deep-baseline.md
```

Install locally:

```bash
cmake --install build --prefix ./install
```

## Repository conventions

- Public API lives under `include/fae/`.
- Narrative documentation lives under `docs/`.
- Tests must cover behavior that changes.
- Public comments should explain the API contract; implementation comments
  should explain non-obvious behavior, not restate code.
- The library is probabilistic by design. Public docs and names must avoid
  claiming proof of equality or inequality.
- Versioned measurement snapshots live under `docs/results/`.
- When a measurement run changes the algorithm decision story, add a new
  `docs/results/YYYY-MM-DD-*.md` file and update both
  `docs/benchmark-baseline.md` and `docs/results/index.md`.
- `scripts/create_results_snapshot.sh` can capture fresh measurement artifacts
  and scaffold a new snapshot markdown file from the template in
  `scripts/templates/`.
- `scripts/promote_results_snapshot.sh` updates the stable baseline entry point
  and archive index when a checked-in snapshot becomes the new current
  baseline.

## Style

- C++20 is the baseline.
- Keep the public API small and coherent.
- Favor deterministic behavior and stable output over cleverness.
- When adding future pairing or folding methods, document the rationale in
  `docs/architecture.md`.
