# ImgTool - Image Processing Utility

A high-performance C++ image processing tool developed as an architecture course project. This project implements two different data structure paradigms (Array of Structs vs. Struct of Arrays) to compare performance characteristics in image manipulation tasks.

## Project Overview

ImgTool is a command-line utility for processing PPM (Portable Pixmap) format images. The project explores how different memory layouts affect performance in image processing operations by providing two parallel implementations:

- **AOS (Array of Structs)**: Traditional approach storing each pixel as a complete struct
- **SOA (Struct of Arrays)**: Modern approach storing separate arrays for each color channel

## Features

The tool supports the following image processing operations:

### 1. **Compress**
Reduces the file size of PPM images while preserving image quality through color palette optimization and frequency analysis.

### 2. **Resize**
Scales images to specified dimensions. Supports both upscaling and downscaling operations.

### 3. **MaxLevel**
Normalizes the color range of an image based on a specified maximum color level. Useful for extending color depth and enhancing image contrast.

### 4. **CutFreq**
Filters colors based on frequency threshold. Removes low-frequency colors (those appearing less frequently) from the image, creating a posterized effect.

### 5. **Info**
Displays detailed information about an image including:
- Dimensions (width × height)
- Maximum color value
- Number of unique colors
- Color frequency statistics

## Project Structure

```
imgtool/
├── common/              # Shared utilities
│   ├── binario.cpp/hpp  # Binary file I/O operations
│   ├── info.cpp/hpp     # Image information utilities
│   └── progargs.cpp/hpp # Command-line argument parsing
├── imgaos/              # AOS (Array of Structs) implementation
│   ├── imageaos.cpp/hpp # Image processing using AOS data structure
│   └── CMakeLists.txt
├── imgsoa/              # SOA (Struct of Arrays) implementation
│   ├── imagesoa.cpp/hpp # Image processing using SOA data structure
│   └── CMakeLists.txt
├── imtool-aos/          # AOS executable
│   ├── main.cpp
│   └── CMakeLists.txt
├── imtool-soa/          # SOA executable
│   ├── main.cpp
│   └── CMakeLists.txt
├── utest-common/        # Unit tests for common utilities
│   ├── utest-common.cpp
│   └── CMakeLists.txt
├── utest-imgaos/        # Unit tests for AOS implementation
│   ├── unitest-imgaos.cpp
│   └── CMakeLists.txt
├── utest-img-soa/       # Unit tests for SOA implementation
│   ├── utest-img-soa.cpp
│   └── CMakeLists.txt
├── ftest-aos/           # Functional tests for AOS
│   ├── ftest-compress.cpp
│   ├── tester.cpp
│   └── compare_*.sh
├── ftest-soa/           # Functional tests for SOA
│   ├── ftest-compress.cpp
│   ├── tester.cpp
│   └── compare_*.sh
├── input/               # Sample input images
│   └── *.ppm
├── output/              # Generated output images
│   └── (output files)
├── CMakeLists.txt       # Main build configuration
├── run_and_build.sh     # Build and test runner script
└── README.md            # This file
```

## Building the Project

### Prerequisites

- **CMake** 3.29 or higher
- **C++20** compatible compiler (GCC, Clang, MSVC)
- **Google Test** (automatically fetched via FetchContent)
- **Microsoft GSL** (Guidelines Support Library, automatically fetched via FetchContent)
- **clang-tidy** (optional, for code quality checks)

### Build Instructions

**Using the provided script:**
```bash
./run_and_build.sh
```

**Manual build:**
```bash
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release
```

The compiled executables will be located in:
- `build-release/imtool-aos/imtool-aos` (AOS version)
- `build-release/imtool-soa/imtool-soa` (SOA version)

## Usage

### Command Syntax

```bash
./imtool-<variant> <input_file> <output_file> <operation> [parameters]
```

Where `<variant>` is either `aos` or `soa`.

### Examples

**Compress an image (AOS):**
```bash
./build-release/imtool-aos/imtool-aos input/lake.ppm output/lake-compressed.cppm compress
```

**Resize an image to 800×600 pixels (SOA):**
```bash
./build-release/imtool-soa/imtool-soa input/lake.ppm output/lake-resized.ppm resize 800 600
```

**Apply frequency cutoff (keep colors appearing more than 50000 times):**
```bash
./build-release/imtool-aos/imtool-aos input/image.ppm output/filtered.ppm cutfreq 50000
```

**Normalize color levels to 16-bit depth (65535 max value):**
```bash
./build-release/imtool-soa/imtool-soa input/image.ppm output/normalized.ppm maxlevel 65535
```

**Display image information:**
```bash
./build-release/imtool-aos/imtool-aos input/image.ppm dummy.ppm info
```

## Data Structure Comparison

### Array of Structs (AOS)
```cpp
struct Pixel {
    int r, g, b;
};
std::vector<Pixel> pixels;
```
**Characteristics:**
- Sequential color channels per pixel
- Better cache locality for single-pixel access
- Less efficient for per-channel operations
- Traditional memory layout

### Struct of Arrays (SOA)
```cpp
struct ImageSOA {
    std::vector<int> r, g, b;
};
```
**Characteristics:**
- Separated color channels
- Better cache efficiency for bulk channel operations
- Vectorization-friendly
- More efficient for algorithms processing specific channels

## Implementation Details

### Core Components

**Image Representation:**
- PPM format support (P6 binary and P3 ASCII)
- Pixel data stored in vectors for dynamic memory management
- Support for variable color depths (8-bit and 16-bit)

**Algorithms:**
- Color frequency analysis using hash maps/unordered maps
- Image resizing using interpolation
- Color space normalization
- Palette optimization

**Code Quality:**
- C++20 standard compliance
- GSL (Guidelines Support Library) for safe C++ practices
- Comprehensive error handling
- clang-tidy integration for static analysis
- Unit and functional test coverage

## Testing

The project includes comprehensive test suites:

**Unit Tests:**
```bash
./build-release/utest-common/utest-common
./build-release/utest-imgaos/unitest-imgaos
./build-release/utest-img-soa/utest-img-soa
```

**Functional Tests:**
```bash
cd ftest-aos && ./tester.cpp
cd ftest-soa && ./tester.cpp
```

Tests validate image processing correctness and compare results between AOS and SOA implementations.

## Performance Profiling

The `run_and_build.sh` script includes examples of performance profiling using `perf` with energy metrics:

```bash
perf stat -a -e power/energy-pkg/ ./build-release/imtool-aos/imtool-aos input.ppm output.ppm compress
```

This allows for comparative analysis of:
- Execution time
- Energy consumption
- Cache behavior
- Memory bandwidth utilization

## Compiler Flags

The project is compiled with strict compiler flags:
- `-Wall -Wextra -Wpedantic` - Enable comprehensive warnings
- `-Werror` - Treat warnings as errors
- `-Wconversion -Wsign-conversion` - Warn about type conversions
- `-fPIC` - Position-independent code

## Supported Formats

**Input:**
- PPM (Portable Pixmap) format, both P3 (ASCII) and P6 (binary)

**Output:**
- PPM format (standard)
- CPPM format (compressed custom PPM format)

## Development Notes

- The project is part of a computer architecture course focusing on data structure optimization
- Both AOS and SOA implementations produce identical results
- Performance differences highlight the importance of memory layout in modern processors
- Code follows C++20 best practices with emphasis on safety and performance

## Author & Context

This is an academic project for a Computer Architecture course (Universidad). The dual implementation approach demonstrates how algorithmic correctness is independent from data structure choices, while performance is significantly affected by memory layout patterns.

## License

This project is developed as academic coursework.
