# NEStalgic 

[![build](https://github.com/zeeps31/nestalgic/actions/workflows/build.yml/badge.svg)](https://github.com/zeeps31/nestalgic/actions/workflows/build.yml)

NEStalgic is (yet another) NES emulator written in C++. It is a personal project and is written for fun.


## Build Instructions

NEStalgic comes with a CMake build script ([CMakeLists.txt](https://github.com/zeeps31/nestalgic/blob/main/CMakeLists.txt)) that can be used on a wide range of platforms. 

### Prerequisites
- **CMake** (Download from [cmake.org](https://cmake.org/download/))
- **C++ Compiler** (GCC, Clang, or MSVC)
- **Google Test** (Automatically handled via CMake)

When building NEStalgic the typical workflow starts with:

```bash
git clone https://github.com/zeeps31/nestalgic.git
cd nestalgic 
cmake -S . -B build # Generate build scripts into a build directory
cmake --build build # build the package
```

## Executing Tests

NEStalgic includes a unit test suite powered by Google Test.

After building the package, tests can be executed as follows:

```bash
ctest --test-dir build/test
```

## Code Quality and Formatting

NEStalgic enforces code style and best practices using:

* **Clang-Tidy** for static analysis and bug detection.
* **Clang-Format** for automatic code formatting.

### Clang-Tidy (Static Analysis)

Checks for Google style, performance, readability, C++ best practices, and bug-prone code.
Errors are treated as warnings (WarningsAsErrors: '*'), failing the build if issues are found.

#### Run Clang-Tidy

```bash
clang-tidy -p build src/cpu.cpp # Single file
clang-tidy -p build include/*.h src/*.cpp test/*.cpp
```

### Clang-Format (Code Styling)

Ensures consistent Google C++ style with a 100-character line limit.

#### Run Clang-Format

```bash
clang-format -i src/cpu.cpp  # Single file
find src/ include/ test/ -name '*.cpp' -o -name '*.h' | xargs clang-format -i # Entire project
```

## License 
This project is licensed under the Apache License – see the [LICENSE](https://github.com/zeeps31/nestalgic/blob/main/LICENSE) file for details.