# Mini Linux Shell with Process Management and Inter-Process Communication

A custom Linux shell developed in C using POSIX system calls. This project demonstrates fundamental operating system concepts, including process creation, command execution, input/output redirection, pipes, background processes, job tracking, and signal handling.

## Project Information

- **Project:** Mini Linux Shell
- **Institution:** Sukkur IBA University
- **Department:** Computer Science
- **Program:** BS Computer Science
- **Section:** F
- **Instructor:** Abbas Mehdi

## Team Members

1. Tahseen Ahmed (Group Leader)
2. Sajid Hussain
3. Mudasir Ali

## Features

- External command execution using `fork()`, `execvp()`, and `waitpid()`.
- Built-in commands: `cd`, `pwd`, and `exit`.
- Input and output redirection (`<`, `>`, `>>`).
- Pipe support (`|`) between two commands.
- Background process execution using `&`.
- Background job tracking.
- Process termination using `kill()`.
- Additional commands: `info`, `mypid`, and `clear`.
- Signal handling for Ctrl+C.

## Project Development Phases

### Phase 1: Basic Shell Structure
- Created the basic shell loop.
- Displayed a command prompt.
- Accepted user input.
- Implemented the exit functionality.

### Phase 2: External Command Execution
- Implemented process creation using `fork()`.
- Used `execvp()` to execute external commands.
- Used `waitpid()` to wait for child processes.

### Phase 3: Built-in Commands and I/O Redirection
- Implemented `cd`, `pwd`, and `exit`.
- Added input redirection using `<`.
- Added output redirection using `>`.
- Added append redirection using `>>`.
- Used `open()`, `dup2()`, and `close()`.

### Phase 4: Pipe Implementation
- Implemented communication between two commands using a pipe.
- Used `pipe()` and `fork()`.
- Redirected standard input and output using `dup2()`.

### Phase 5: Background Process Execution
- Added support for background commands using `&`.
- Allowed the shell to continue without waiting for background processes.
- Used `waitpid()` with `WNOHANG` for process cleanup.

### Phase 6: Job Tracking and Process Termination
- Implemented background job tracking.
- Stored process IDs and command information.
- Added process termination using `kill()`.

### Phase 7: Additional Features and Signal Handling
- Added `info`, `mypid`, and `clear`.
- Implemented Ctrl+C handling using `sigaction()`.
- Configured signal behavior for child processes.

## Requirements

- Linux operating system (Kali Linux, Ubuntu, or Debian).
- GCC compiler.
- Basic terminal knowledge.

## Installation and Execution

### 1. Clone the Repository

```bash
git clone https://github.com/TahseeAhmed/Mini-Linux-Shell.git
```

### 2. Enter the Project Directory

```bash
cd Mini-Linux-Shell
```

### 3. Compile the Program

```bash
gcc -Wall -Wextra shell.c -o shell
```

### 4. Run the Shell

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
└── .gitignore
```

## Limitations

- Supports one pipe between two commands.
- Uses simple command tokenization.
- Does not support advanced shell features such as complex quoting and escaped spaces.
- Does not implement full Bash-style job control.

## Technologies Used

- C Programming Language
- Linux / Kali Linux
- GCC Compiler
- POSIX System Calls
- Git and GitHub

## Repository

GitHub: https://github.com/TahseeAhmed/Mini-Linux-Shell

## Academic Institution

Sukkur IBA University  
Department of Computer Science  
Pakistan
