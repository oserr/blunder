# blunder

A chess engine written in modern C++.

## Compiling

Currently, blunder only depends on a modern C++ compiler with support for C++23, such as
`gcc` 13.1.1 or `clang` 16 or greater, and a more recent version of `cmake`. With these
depencies installed, simply run the following command:

```sh
cd blunder
mkdir build
cd build
cmake ..
make
```

## Notes

This is in super early stage development. The only artifacts produced by the project are:
* `bbprinter`: an executable to print common bitboards.
* `genmagics`: an executable to generate magic numbers for sliding pieces.

At the moment, these artificts are meant to help with development and debugging, and as
foundation for move generation.
