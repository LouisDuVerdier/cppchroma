# cppchroma

cppchroma is a C++ project allowing to run interactive commands (e.g., bash) while colorizing the output according to patterns and configuration.

Example when running a session with `./cppchroma bash` and then typing `ifconfig`:

![Example of visualization](https://github.com/LouisDuVerdier/cppchroma/raw/main/docs/cppchroma_example.png "Example of visualization")

This tool is meant to work on Linux-like systems, and may work anywhere you can run commands like bash or zsh (VSCode terminal, WSL, etc.). It is basically a wrapper than will colorize what it runs.

An example of config would be to set the default command to run on VSCode terminal to `~/path/to/cppchroma bash`. This way, each time you create a new terminal, you'll always have colors without needing to think about it.

For safety and efficiency reasons, colorization is done through Google's RE2 regex library, which is slightly more limited than what is covered by regular regex engines, but that should cover most cases.

This project was greatly inspired by https://github.com/hSaria/ChromaTerm/ that is now archived, but designed to be faster. The goal of cppchroma was to be able to work on sessions where apps may be printing lots of logs, without impacting their speed (I/O throttling). With the default config, cppchroma adds very little overhead compared to the same commands ran without it, and is at least 10 times faster than ChromaTerm, even with its --pcre option.

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

At first run, a `~/.cppchroma_config.yaml` file is created, containing a [default configuration](https://github.com/LouisDuVerdier/cppchroma/blob/main/include/defaultconfig.h) that you may modify according to your needs.

You may set `CPPCHROMA_CONFIG_PATH` environment variable to load a custom path instead.

**Note:** RE2 library only supports limited range of regex features. Its syntax may be found here: https://github.com/google/re2/wiki/Syntax. Multiple online tools are available to tests RE2 syntax, in case you wish to add your own rules.

Very simple example of config to highlight a few keywords, case non-sensitive:

```yaml
rules:
- description: Error
  regex: (?i)\b(error|down|bad|fail(ure)?)\b
  color: f#F44336

- description: Warning
  regex: (?i)\b(warn(ing)?s?|not?)\b
  color: f#CDDC39

- description: Success
  regex: (?i)\b(yes?|running|good|up|ok(ay)?)\b
  color: f#33D375
```

For colors, 'f' or 'b' prefix may be used to define either background or foreground color.

Right now, cppchroma's configs are only covering a subset of what ChromaTerm may offer. More advanced customizations may be requested if needed; I'll then see what I can do.
