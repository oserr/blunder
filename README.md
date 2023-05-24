# blunder

A chess engine written in modern C++.

## Dependencies

* A modern C++ compiler with support for C++23, such as `gcc` 13.1.1 or `clang`
  16 or greater.
* A recent version of `cmake`, at least 3.23, to generate the build files
  without having to change CMakeLists.txt.
* A generator supported by CMake that can be used for compiling C++, such as
  `make` or `ninja`.
* A unix-like environment, e.g. Linux, MacOS. Support for Windows will be added
  later.

## Compiling

Assuming that all the dependencies are installed and `make` is the default build
tool, the blunder library and executables can be built by running the following
commands:

```sh
cd blunder
mkdir build
cd build
cmake ..
make
```

## Notes

This is in super early stage development. The only artifacts produced by the
project are:

* `bbprinter`: an executable to print common bitboards.
* `genmagic`: an executable to generate magic numbers for sliding pieces.

At the moment, these artificts are meant to help with development and debugging,
and as foundation for move generation.
