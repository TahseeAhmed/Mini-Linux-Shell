# Mini Linux Shell

A simple Linux shell developed in C using POSIX system calls. This project demonstrates process management, command execution, I/O redirection, pipes, background processes, job tracking, and signal handling.

## Features

- Command execution using `fork()`, `execvp()`, and `waitpid()`
- Built-in commands: `cd`, `pwd`, and `exit`
- Input and output redirection (`<`, `>`, `>>`)
- Pipe support (`|`) between two commands
- Background process execution (`&`)
- Background job tracking
- Process termination using `kill`
- Additional commands: `info`, `mypid`, and `clear`
- Signal handling for Ctrl+C

## Requirements

- Linux operating system (Kali Linux, Ubuntu, Debian, etc.)
- GCC compiler
- Basic terminal

## Installation

Clone the repository:

```bash
git clone https://github.com/YOUR_USERNAME/Mini-Linux-Shell.git
```

Enter the project directory:

```bash
cd Mini-Linux-Shell
```

Compile the program:

```bash
gcc -Wall -Wextra shell.c -o shell
```

Run the shell:

```bash
./shell
```

## Example Commands

```bash
pwd
ls
cd ..
cat file.txt
ls | grep txt
sleep 10 &
mypid
info
clear
exit
```

## Project Structure

```text
Mini-Linux-Shell/
├── shell.c
├── README.md
├── .gitignore
└── LICENSE
```

## Limitations

- Supports one pipe between two commands.
- Uses simple command tokenization.
- Does not implement full Bash job control.

## Contributors

- Tahseen Ahmed
- Sajid Hussain
- Mudasir Ali

## Institution

Sukkur IBA University  
Department of Computer Science

## License

MIT License (see LICENSE file).
