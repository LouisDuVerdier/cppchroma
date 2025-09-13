# cppchroma

cppchroma is a C++ project allowing to run interactive commands (e.g., bash) while colorizing the output according to patterns and configuration.

It is meant to work on Linux-like systems.

Colorization is done through Google's RE2 regex library.

Greatly inspired by https://github.com/hSaria/ChromaTerm/, but designed to be faster.

## Setup Instructions

```
mkdir build
cd build
cmake ..
make
```

## Usage

```
./cppchroma <command> <arguments>
```

For example to colorize the output of a bash session:
```
./cppchroma bash
```
