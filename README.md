# mC Compiler

This repository holds a code-base for the [compiler construction course](https://github.com/W4RH4WK/UIBK-703602-Compiler-Construction).

A more detailed overview can be found in [`doc/overview.md`](doc/overview.md).

## Input Language

- see [mC Specification](https://github.com/W4RH4WK/UIBK-703602-Compiler-Construction/blob/master/mC_specification.md)
- see [examples](doc/examples)

## Prerequisites

- [Meson](http://mesonbuild.com/) in a recent version (`0.44.0`)
  (you may want to install it via `pip3 install --user meson`)
- [Ninja](https://ninja-build.org/)
- `time`, typically located at `/usr/bin/time`, do not confuse with the Bash built-in
- `flex` for generating the lexer
- `bison` for generating the parser
- a compiler supporting C11 (and C++14 for unit tests) -- typically GCC or Clang
- `pdftk` for the generation of CFG - pdfs (see CFG paragraph below)

## Building and Testing

Generate the build directory.
[Google Test](https://github.com/google/googletest) is used for unit testing.
Meson downloads and builds it automatically if it is not found on your system.

    $ meson builddir
    $ cd builddir

Meson creates Ninja build files by default.
Let's build.

    $ ninja

Unit tests can be run directly with Ninja (or Meson).

    $ ninja test

For integration testing we pass all example inputs to the compiler and observe its exit code.
This is automated by a Bash script.

    $ ../test/integration

For testing the assembler code we originally were using a self written testing framework in the mC
language (/doc/test_mCc/test1.mC, see /doc/test_mCc/usage.txt for info on how to run test).
We then further on added a few google tests, which only compare outputs for some simple code snippets.

For more information how to use the compiler, please try
    $ ./mcC --help

To generate a CFG for a mC program please use mC_to_cfg like in the following:
   ./mC_to_cfg %yourfile% > cfg.txt && dot -Tpdf cfg.txt | csplit --quiet --elide-empty-files --prefix=tmpx - "/%%EOF/+1" "{*}" && pdftk tmpx* cat output cfg.pdf && rm -f tmpx*g

This generates a pdf called 'cfg.pdf' with one cfg per function/page.
Make sure to replace %yourfile% with a valid mC program, eg. ../doc/examples/fib.mC
We had some problems using dot to generated multiple graphs in one picture, this method attaches all pictures into a single pdf.
You might need to install pdftk for this to work.

## Known Issues

- The compiler optimization are not implemented

- Type checking is incomplete for arrays. You can for instance add an array and a primitive as long both have the same type. 
```
void main(){int[3] a; int b; b = a;} 
```
will lead to an error and also an reassignment of an array will result in an error.
