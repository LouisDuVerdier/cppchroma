# cppchroma

cppchroma is a C++ project allowing to run interactive commands (e.g., bash) while colorizing the output according to patterns and configuration.

It is meant to work on Linux-like systems.

Colorization is done through Google's RE2 regex library.

Greatly inspired by https://github.com/hSaria/ChromaTerm/ that is now archived, but designed to be faster.

![Example of visualization](https://github.com/LouisDuVerdier/cppchroma/raw/main/docs/cppchroma_example.png "Example of visualization")

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

For example, to colorize the output of a bash session:
```
./cppchroma bash
```

## Configuration

At first run, a `~/.cppchroma_config.yaml` file is created, containing a default configuration that you may modify according to your needs.

You may set `CPPCHROMA_CONFIG_PATH` environment variable to load a custom path instead.

**Note:** RE2 library only supports limited range of regex features. Its syntax may be found here: https://github.com/google/re2/wiki/Syntax. Multiple online tools are available to tests RE2 syntax, in case you wish to add your own rules.
