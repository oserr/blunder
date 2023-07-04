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
* Google Test for unit testing.
* The [pytorch](https://pytorch.org/) c++ frontend. The easiest way to get this
  working is to install the library via a system package manager, e.g. `pacman`
  on archlinux. If you have a custom install of the library in a non-default
  location, e.g. `${HOME}/opt/libtorch`, then exporting
  `TORCH_INSTALL_PREFIX=${HOME}/opt/libtorch`, or setting the variable when
  running cmake, will allow the build the work. For development, I'm using Torch
  version 2.1 with CUDA 12.1, which is currently the latest release.

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
