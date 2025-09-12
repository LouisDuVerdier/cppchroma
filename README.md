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

## Limitations

Depending on the system, cppchroma may have issues reading the full output of the `<command>` after it finishes. This is due to EIO error being received before the fd of the forkpty() gets fully drained. There is no available workaround for now, but remains a rare case. If running for example a bash or similar session, that shouldn't be an issue.
