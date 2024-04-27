# CUDAKernels
A collection of CUDA kernels I wrote, just for fun

## Future work
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
