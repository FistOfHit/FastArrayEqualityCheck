# TODO

This file tracks the highest-value follow-up work for `FAE`.

The repository is in a good structural state. The remaining work is mostly
research, evaluation, and documentation depth rather than build-system cleanup.

## 1. Research Prior Art For Locality-Sensitive Array Fingerprints

Goal:
Understand how much of the original "butterfly-like reduction into a compact
similarity-preserving value" idea already exists in the literature and in
open-source systems, and decide where `FAE` is genuinely new versus where it is
another instance of an existing family.

Questions to answer:

- Is `FAE` best described as a form of locality-sensitive hashing, fuzzy
  hashing, sketching, or something else?
- Which existing systems are the closest conceptual matches for "compact value
  representing a larger structure, where closeness of the value approximates
  closeness of the source"?
- Does the original butterfly-reduction idea map cleanly onto known techniques,
  or is it mostly an implementation strategy layered on top of standard LSH /
  fuzzy-hash ideas?

Specific systems and families to survey:

- SimHash and related cosine-similarity fingerprints
- MinHash and MinHash LSH
- Fuzzy hashes such as `ssdeep`
- Trend Micro `TLSH`
- Vector-search / ANN systems that use hashing or sketching layers as coarse
  filters
- Rolling-hash / Rabin-fingerprint style systems where compact fingerprints are
  used for content identity, even if they are not locality-sensitive in the
  same way

Concrete deliverables:

- Add `docs/prior-art.md`
- For each candidate family / project, document:
  - what it fingerprints
  - what similarity notion it preserves, if any
  - whether it is exact or probabilistic
  - whether it produces one compact value or an index-oriented structure
  - which languages / repos / libraries exist
  - how it differs from `FAE`
- Add a short conclusion section that states whether `FAE` should position
  itself as:
  - an LSH-inspired array fingerprint library
  - a fuzzy-hashing library for numeric arrays
  - a general probabilistic sketching library
  - or a smaller, narrower experimental library

## 2. Benchmark Against Real Equality Baselines

Goal:
Stop measuring only `FAE` against itself. Add direct comparisons against
ordinary equality mechanisms so the performance story answers the real user
question: "When should I use this instead of normal equality?"

Baselines to add:

- `std::equal`
- direct element-wise loop
- `std::vector<T>::operator==` where applicable
- `memcmp` for trivially comparable integral buffers when the comparison is
  valid and well-defined

Scenarios to benchmark:

- one-off equality check on equal arrays
- one-off equality check on unequal arrays with an early mismatch
- one-off equality check on unequal arrays with a late mismatch
- repeated compare-many-against-one, where fingerprints can be precomputed
- cached-fingerprint compare, to isolate the cost of comparing the compact
  representation instead of recomputing it

Metrics to capture:

- latency per comparison
- throughput
- effect of array size
- effect of element type
- effect of mismatch position
- any allocation overhead on the `FAE` path

Documentation work:

- Update `docs/empirical-evaluation.md`
- Add a section to `README.md` summarizing where `FAE` wins and where a direct
  comparison is still the better tool
- Make the distinction explicit between:
  - fingerprint construction cost
  - fingerprint comparison cost
  - direct array equality cost

## 3. Write Deep Algorithm Documentation

Goal:
Explain the library in enough depth that a reader can understand what each
algorithm is doing, why it exists, and where it fits relative to the original
butterfly-reduction vision.

Add a new document:

- `docs/algorithms.md`

Topics to cover:

- The common fingerprint pipeline:
  - integral span input
  - normalization to `uint64_t`
  - type tagging
  - seed usage
  - final `Fingerprint { low, high, length }` structure
- `hash_combine`:
  - per-element ordered mixing
  - the role of the two lanes
  - what kinds of structure it preserves or destroys
- `tuple_mix`:
  - adjacent-pair processing
  - odd-tail handling
  - why pairwise local structure might help
- `cantor_mix`:
  - splitting into 16-bit chunks
  - exact Cantor pairing inside a larger probabilistic mixer
  - tradeoffs versus simpler rolling combine
- `legacy_fold`:
  - how repeated pairwise reduction works
  - why it reflects the repo's original prototype direction
  - why it is no longer the default
- Advanced primitives:
  - `splitmix64`
  - `fae::pairing::szudzik_pair`
  - `fae::reduction::pairwise_fold64`
- A section called "What `FAE` Means By Locality" that explicitly states:
  - what locality-sensitive behavior is actually implemented today
  - what is only aspirational from the original repo concept
  - what would need to change to make the locality-sensitive claim stronger

Nice-to-have additions:

- pseudocode
- small worked examples
- diagrams for reduction / mixing flow

## 4. Revisit The Original Butterfly / Similarity Vision

Goal:
Decide whether `FAE` should continue as "probabilistic equality fingerprinting"
or evolve back toward the original vision of a butterfly-like reduction whose
output supports approximate matching under a clearly defined distance metric.

Questions to answer:

- What similarity metric do we actually care about?
  - exact equality
  - Hamming-like closeness
  - edit / shift robustness
  - value-wise numeric proximity
- Can a butterfly-style reduction preserve a useful notion of locality, or does
  it mostly behave like an ordinary hash unless designed very carefully?
- Should the long-term API expose distance thresholds rather than only
  `fingerprints_match` / `fingerprints_differ`?

Possible outputs:

- an architecture note rejecting the original direction
- an experimental branch with a true locality-sensitive candidate
- a separate API tier for "similarity search" rather than "fingerprint match"
