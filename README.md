# `xpic-processor`

> Modern C++20 image-processing toolkit and CLI utilities for JPEG manipulation and deinterlacing.

`xpic-processor` provides a modern C++ wrapper around `libjpeg` together with command-line tools for image processing workflows.

---

## Features

- Modern C++20 codebase
- JPEG processing built on top of `libjpeg`
- Image deinterlacing utilities
- Docker-based reproducible development environment
- Automated local CI workflow
- Doxygen documentation generation
- Valgrind integration
- Coverage report generation

---

## Requirements

| Dependency | Version |
|---|---|
| GCC | 15+ |
| CMake | 3.20+ |
| Docker | Latest |
| libjpeg | Required |

---

## Quick Start

### Build the Docker Image

```bash
cd ubuntu-resolute-gcc-15

docker build -t xpic:1.0 .
```

---

### Launch the Development Container

Run the following command from the project root directory:

```bash
docker run \
    --name=xpicc \
    --hostname=cypher \
    -v $PWD:/home/cxx-core \
    --net=host \
    --restart=no \
    -it xpic:1.0 /bin/bash
```

---

## Build & Test

Inside the container:

```bash
cd /home/cxx-core

./local-ci.sh
```

The local CI pipeline performs the following tasks:

- Configure and build the project
- Execute unit tests
- Generate coverage reports
- Generate Doxygen documentation
- Execute Valgrind analysis

---

## CLI Usage

### Display Help Menu

```bash
deinterlacer --help
```

---

### Deinterlace an Image

```bash
deinterlacer \
    -i rc/interlaced.jpg \
    -o output/deinterlaced.jpg
```

---

## Logs

Application logs are available in:

```bash
logs/deinterlacer.log
```

Display logs:

```bash
cat logs/deinterlacer.log
```

---

## Documentation & Reports

Generated artifacts are available under the `reports/` directory.

### Coverage Report

Open in your browser:

```text
reports/coverage_html/index.html
```

---

### Doxygen Documentation

Open in your browser:

```text
reports/html/index.html
```

---

### Valgrind Report

Open in your browser:

```text
reports/valgrind-report.html
```

---

### Quality Report

```text
reports/deinterlacer-quality-report.log
```

---

## Project Structure

```text
.
├── include/                  # Public headers
├── src/                      # Source files
├── tests/                    # Unit tests
├── reports/                  # Generated reports
├── logs/                     # Runtime logs
├── rc/                       # Resource files
├── ubuntu-resolute-gcc-15/   # Docker environment
├── docs/                     # Documentation
└── CMakeLists.txt
```

---

## Development Workflow

Recommended development workflow:

1. Build the Docker image
2. Launch the development container
3. Execute the local CI pipeline
4. Inspect generated reports and logs

---

## Design Goals

- Clean and maintainable C++20 architecture
- Safe abstractions around `libjpeg`
- Reproducible builds and tooling
- Automated quality analysis
- Extensible image-processing pipeline

---

## Roadmap

- [ ] PNG support
- [ ] SIMD optimizations
- [ ] Batch processing support
- [ ] GPU acceleration
- [ ] C++23 modules support
- [ ] Additional image filters

---

## License

Licensed under the MIT License.

See `LICENSE` for details.
