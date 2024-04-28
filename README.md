# FastArrayEqualityCheck
A sub-linear equality check for large arrays.

## Sub-linear in time (but not in space)
Elementwise comparison is fast, but for very large arrays, a less direct comparison of array elements could be faster. This library maps pairs of elements to a single number and then hashes it, repeating this process until all elements of an array have been processed into a single hash. Once arrays A and B have been hashed, A == B becomes instant.

## Installation

## Usage

## Contributing

## License

## Future work
- modify butterfly pattern for log(n) compute
- Verify LSH algorithm and determine probability of collisions for large numbers of trials
- Make python library and pybinds
- Scale to all available GPUs
- option to run only on some GPUs
- CPU implementation + comparative benchmarks
- automatically compile to arch of GPUs and CUDA version
- Wrapper bash scripting for testing and benchmarking
- pypy
- compile on pip install
- smarter tiling and hashing with small fragments to avoid host-device transfers
- adapt to arrays stored in disk - mmap and then load on demand
- Develop a GPU optimized hashing algorithm - draw ideas from crypto mining perhaps?
