# soft-ieee754

This is a software implementation of the ieee754 floating point format for any
size with any configuration for exponent, mantissa and bias values, allowing you
to create any size floating point format when unavailable by the hardware.

Notable examples are 'half-precision' floating points (also known as binary16)
used in graphic processing hardware not available on modern CPUs, or needed for
compact network transmission of floating points.

## Why?

Most hardware today only supports 32bit (single precision) and 64bit (double
precision) floating points in hardware, without support for other sizes that are
useful when compactness is needed.

## Status

Currently the library is capable of supporting almost correct arithmetic
operations and is binary compatible with other floating point implementations of
same requested size (for example, binary32).

### Known bugs / missing features:
 * Algebraic functions are not implemented yet
 * Floating points larger than 64bit cannot be represented
 * Arithmetic operations with infinity results into incorrect results
 * Casting of ±NaN or ±Infinity into a signed or unsigned is incorrect

### Testing

To compile the unit tests you'll need `libgtest`. To compile, run:

    g++ -O2 -Iincludes -std=c++11 tests/*.cpp -lgtest -lgtest_main -lpthread -o soft-ieee754-test

Then run the executable `soft-ieee754-test`
