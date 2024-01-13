# Floating-Point Conversion Tool

**Table of Contents**
- [Introduction](#introduction)
- [Usage](#usage)
- [Command Line Options](#command-line-options)
- [File Descriptions](#file-descriptions)
- [Example Input and Output](#example-input-and-output)

## Introduction

This repository contains two C programs:

- `float-2-hex.c`: Converts decimal floating-point numbers to their hexadecimal representation.
- `hex-2-float.c`: Converts hexadecimal floating-point representations back to decimal form.

These programs support various floating-point formats, including single-precision, double-precision, half-precision, and custom formats with user-defined exponent and fraction sizes. You can also enable verbose output to display additional information about the conversions.

## Usage

To compile and run the programs, follow these steps:

1. Clone the repository.
2. Compile the desired program using a C compiler (e.g., gcc):

```shell
gcc float-2-hex.c -o float2hex
gcc hex-2-float.c -o hex2float
```

# Command Line Options

## `float-2-hex.c` (Convert Decimal Float to Hexadecimal)
- `-f`: Convert input into C standard float format (default).
- `-d`: Convert input into C standard double format.
- `-H`: Display help message.

## `hex-2-float.c` (Convert Hexadecimal to Decimal Float)
- `-i <input_file>`: Specify the input file containing hexadecimal representations (default: stdin).
- `-d`: Convert double-precision (64-bit) hexadecimal numbers.
- `-h`: Convert half-precision (16-bit) hexadecimal numbers.
- `-b`: Convert single-precision (32-bit) hexadecimal numbers (default).
- `-m`: Convert 8-bit hexadecimal numbers.
- `-e <BITS>`: Use BITS bits for the exponent (default: 8 for single-precision, 11 for double-precision).
- `-E <BIAS>`: Use BIAS as the exponent bias (default: 127 for single-precision, 1023 for double-precision).
- `-f <BITS>`: Use BITS bits for the fraction (default: 23 for single-precision, 52 for double-precision).
- `-F <BITS>`: Use BITS bits for the fraction (same as `-f`).
- `-v`: Enable verbose output.
- `-H`: Print help message.

# File Descriptions

- `float-2-hex.c`: Converts decimal floating-point numbers to their hexadecimal representation.
- `hex-2-float.c`: Converts hexadecimal floating-point representations back to decimal form.
- `README.md`: This README file.

# Example Input and Output

For example input and output, see the provided examples in the respective sections of the code files (`float-2-hex.c` and `hex-2-float.c`).
