# MSH — Minimal Unix Shell in C

## 1. Project Information

**Project:** MSH — Minimal Unix Shell  
**Course:** ICS 2305 — Systems Programming  
**Student Name:** PHILLIP KIOKO KIMONYI  
**Student ID:** SCT212-0057/2024
**Language:** C 
**Platform:** Ubuntu Linux

## 2. Project Description

MSH is a small command-line shell developed in C for the ICS 2305 Systems Programming practical assignment. Its purpose is to demonstrate how a 
Unix-like operating system starts and manages processes.

The shell accepts commands from the user, separates commands into arguments, executes external programs, and reports their exit status. It also provides 
built-in commands, background execution, output redirection and basic signal handling.

## 3. Features Implemented

The following checklist records the implementation status. Confirm each item against the test plan before submission.

| Task | Feature | Status |
|---|---|---|
| 1 | Read input using `getline()` and exit cleanly on Ctrl-D | Complete, subject to testing |
| 2 | Split commands into arguments and ignore extra whitespace | Complete, subject to testing |
| 3 | Execute external commands using `fork()`, `execvp()` and `waitpid()` | Complete, subject to testing |
| 4 | Implement `cd`, `help` and `exit` | Complete, subject to testing |
| 5 | Display the previous command's exit status | Complete, subject to testing |
| 6 | Support background execution and reap completed jobs | Complete, subject to testing |
| 7 | Support output redirection using `>` | Complete, subject to testing |
| 8 | Handle Ctrl-C without terminating the shell | Complete, subject to testing |

## 4. Building and Running

### Requirements

- Ubuntu or another Linux environment.
- GCC compiler.
- GNU Make.

### Build

Open a terminal in the project directory and run:

```bash
make
```

Alternatively, compile directly:

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -g -o msh msh.c
```

### Run

```bash
./msh
```

### Clean build files

```bash
make clean
```

## 5. Design Explanation

The shell uses the process-management system calls provided by Unix. When a user enters an external command, the program first separates the command and its arguments. It then calls `fork()` to create a child process. The child process uses `execvp()` to replace its program with the requested command. If execution fails, it reports the error and exits with an appropriate status. The parent process uses `waitpid()` to obtain the result of a foreground command and update the status displayed in the next prompt.

The commands `cd`, `help` and `exit` are handled directly by the shell instead of being started as external programs. The `cd` command must change the working directory of the shell itself. If it ran in a child process, only that child would change directory, while the parent shell would remain in its previous location. The `exit` command must also terminate the shell process, which is why it is implemented internally.

For background commands, the shell starts a child process and returns to the prompt without waiting for that command to finish. It stores the child's process ID and periodically checks whether the process has completed. It uses `waitpid()` with `WNOHANG` to collect completed background processes. This prevents finished child processes from remaining as zombies.

Output redirection is performed inside the child process. The shell opens the requested file, uses `dup2()` to connect standard output to that file, and closes the original file descriptor. For Ctrl-C, the shell ignores `SIGINT`, while the child restores the default signal behaviour before executing an external program.

## 6. Known Limitations

- The shell uses simple whitespace-based argument parsing. It does not support quoted arguments containing spaces.
- It does not implement pipes, input redirection, append redirection or command substitution.
- It does not implement full interactive job control, such as `fg`, `bg` and terminal process-group management.
- Built-in commands are not supported with background execution or output redirection.
- The background-job list has a fixed maximum capacity.
- Environment-variable expansion is not implemented.

## 7. Testing

The test cases and their observed results are recorded in:

`tests/test_plan.md`

The shell should be tested using the supplied cases before final submission. The results in that file must reflect actual execution.

## 8. Sources and AI-Tool Declaration

### Technical references

- ICS 2305 Systems Programming Practical Assignment, 13 November 2026.
- Linux manual pages: `getline(3)`, `strtok(3)`, `fork(2)`, `execvp(3)`, `waitpid(2)`, `chdir(2)`, `getenv(3)`, `open(2)`, `dup2(2)`, `poll(2)` and `signal(7)`.
- GNU GCC documentation and GNU Make documentation.

### AI-tool declaration

ChatGPT and Deepseek was used as a learning and developing and most importantly to debug the erroneous code
i also referred to the lecturers notes especially when creating the bash
i also used the ai to refine my Readme file

author.    Phillip Kimonyi      computer technology

