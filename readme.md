
# 🐚 CShell-CShore

  

A simple Unix shell written in C. **CShell-CShore** is a lightweight command-line shell that reads user input, tokenizes commands, and executes basic shell operations.

  

## Features

- Reads and parses user input commands.

- Supports quoted arguments (e.g., `"ls -la"` is treated as a single argument).

- Implements a simple command-line interface with a working directory prompt.

- Debug mode for verbose output (`--debug` flag).

- Has command history accesible by using arrow up or down keys.

- Persistent command history saved to `~/.cshell_history`.

  

## Installation

1. Clone the repository:

```sh

git clone https://github.com/soham004/Cshell-Cshore.git

cd CShell-CShore

```

2. Compile the shell:

```sh

make

```

  

## Usage

Run the shell:

```sh

./cshell

```

  

Run in debug mode:

```sh

./cshell  --debug

```

  

The shell will display the current working directory followed by `>` as a prompt, allowing you to enter commands.

  
  

## Roadmap

-  :heavy_check_mark: Add support for executing system commands via `shell_execute`. 
-  :heavy_check_mark: Support built-in commands like `cd`, `exit`, etc.
-  :heavy_check_mark: Implement history.
-  :heavy_check_mark: Implement persistent history.
- Add support for auto completion for file names on `Tab`.
- Add coloured output for better readibility.


## License

This project is licensed under the [Unlicense](https://unlicense.org).