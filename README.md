# FastArrayEqualityCheck

Fast equality checking for large arrays.

## Sub-linear in time (but not in space)

Elementwise comparison is fast, but for very large arrays, a less direct comparison of array elements could be faster. This library maps pairs of elements to a single number and then hashes it, repeating this process until all elements of an array have been processed into a single hash. Once arrays A and B have been hashed, A == B becomes instant.

## Building

### Prequisites

- C++20 compatible compiler (e.g., GCC 10+, Clang 10+, MSVC 2019+)
- CMake 3.15 or higher
- Google Test framework

### Build steps

- Clone the repository:

    ```Bash
    git clone https://github.com/yourusername/FastArrayEqualityCheck.git
    cd FastArrayEqualityCheck
    ```

- Run make to build:

```Bash
make all
```

### Running Tests

After building the project, you can run the tests using:

```Bash
make test
```

## Usage

## Contributing

We welcome contributions to the FastArrayEqualityCheck project! Whether you're
fixing bugs, improving documentation, or proposing new features, your efforts
are appreciated.

### Getting Started

- Fork the repository on GitHub.
- Clone your fork locally.
- Create a new branch for your contribution.
- Make your changes and commit them with a clear, descriptive commit message.
- Push your changes to your fork on GitHub.
- Submit a pull request to the main repository.

### Guidelines

- Follow the existing code style and conventions.
- Write clear, concise commit messages.
- Add tests for new features or bug fixes.
- Update documentation as necessary.
- Be respectful and constructive in discussions and code reviews.

If you're unsure about anything, don't hesitate to open an issue to discuss
your ideas or ask questions.
Thank you for considering contributing to FastArrayEqualityCheck. Your
involvement helps make this project better for everyone!

## License

This project is licensed under the MIT License. See the LICENSE file
for details.

## Future work

- Verify LSH algorithm and determine probability of collisions for large numbers of trials
- Make python library and pybinds
- Scale to all available GPUs
- option to run only on some GPUs
- CPU implementation + comparative benchmarks
- automatically compile to arch of GPUs and CUDA version
- pypy
- compile on pip install
- smarter tiling and hashing with small fragments to avoid host-device transfers
- adapt to arrays stored in disk - mmap and then load on demand
- Develop a GPU optimized hashing algorithm - draw ideas from crypto mining perhaps?
