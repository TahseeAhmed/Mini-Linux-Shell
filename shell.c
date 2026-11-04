#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

#define MAX_INPUT 1024
#define MAX_ARGS 100
#define MAX_JOBS 100

// Background job structure
struct Job {
    pid_t pid;
    char command[MAX_INPUT];
};

struct Job jobs[MAX_JOBS];
int job_count = 0;

// Signal flag
volatile sig_atomic_t interrupted = 0;

// Handle Ctrl+C
void handle_sigint(int sig) {
    (void)sig;
    interrupted = 1;
}

// Add background job
void add_job(pid_t pid, char *command) {
    if (job_count < MAX_JOBS) {
        jobs[job_count].pid = pid;
        strncpy(jobs[job_count].command, command, MAX_INPUT - 1);
        jobs[job_count].command[MAX_INPUT - 1] = '\0';
        job_count++;
    } else {
        printf("Job list is full.\n");
    }
}

// Check completed background jobs
void check_jobs() {
    for (int i = 0; i < job_count; i++) {
        int status;
        pid_t result = waitpid(jobs[i].pid, &status, WNOHANG);

        if (result == jobs[i].pid ||
            (result == -1 && errno == ECHILD)) {

            printf("\n[Completed] PID: %d | %s\n",
                   jobs[i].pid, jobs[i].command);

            for (int j = i; j < job_count - 1; j++) {
                jobs[j] = jobs[j + 1];
            }

            job_count--;
            i--;
        }
    }
}

// Display background jobs
void show_jobs() {
    check_jobs();

    if (job_count == 0) {
        printf("No background jobs.\n");
        return;
    }

    printf("\nBackground Jobs:\n");

    for (int i = 0; i < job_count; i++) {
        printf("PID: %d | Command: %s\n",
               jobs[i].pid, jobs[i].command);
    }
}

// Terminate a background job
void terminate_job(pid_t pid) {
    for (int i = 0; i < job_count; i++) {
        if (jobs[i].pid == pid) {
            if (kill(pid, SIGTERM) == 0) {
                printf("Termination requested for PID %d.\n", pid);
            } else {
                perror("kill");
            }
            return;
        }
    }

    printf("PID %d not found in background jobs.\n", pid);
}

// Execute command with redirection
void execute_command(char **args, int input_fd, int output_fd) {

    int in = input_fd;
    int out = output_fd;

    for (int i = 0; args[i] != NULL; i++) {

        if (strcmp(args[i], "<") == 0) {
            if (args[i + 1] == NULL) {
                fprintf(stderr, "Missing input filename.\n");
                exit(EXIT_FAILURE);
            }

            in = open(args[i + 1], O_RDONLY);

            if (in < 0) {
                perror("Input file");
                exit(EXIT_FAILURE);
            }

            args[i] = NULL;
            break;
        }

        if (strcmp(args[i], ">") == 0 ||
            strcmp(args[i], ">>") == 0) {

            int flags = O_WRONLY | O_CREAT;

            if (strcmp(args[i], ">>") == 0) {
                flags |= O_APPEND;
            } else {
                flags |= O_TRUNC;
            }

            if (args[i + 1] == NULL) {
                fprintf(stderr, "Missing output filename.\n");
                exit(EXIT_FAILURE);
            }

            out = open(args[i + 1], flags, 0644);

            if (out < 0) {
                perror("Output file");
                exit(EXIT_FAILURE);
            }

            args[i] = NULL;
            break;
        }
    }

    if (in != STDIN_FILENO) {
        if (dup2(in, STDIN_FILENO) < 0) {
            perror("dup2 input");
            exit(EXIT_FAILURE);
        }
        close(in);
    }

    if (out != STDOUT_FILENO) {
        if (dup2(out, STDOUT_FILENO) < 0) {
            perror("dup2 output");
            exit(EXIT_FAILURE);
        }
        close(out);
    }

    execvp(args[0], args);

    perror("Command execution failed");
    exit(EXIT_FAILURE);
}

// Display project information
void show_info() {
    printf("\n====================================\n");
    printf("       MINI LINUX SHELL\n");
    printf("====================================\n");
    printf("Project: Mini Linux Shell with Process Management\n");
    printf("         and Inter-Process Communication\n");
    printf("Language: C\n");
    printf("Operating System: Linux\n");
    printf("Features: fork, exec, wait, pipes, redirection\n");
    printf("          background jobs and process management\n");
    printf("====================================\n\n");
}

// Main function
int main() {

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));

    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);

    // Do not use SA_RESTART: allow Ctrl+C to interrupt fgets/waitpid
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        return EXIT_FAILURE;
    }

    // Keep Ctrl+Z from suspending the shell itself
    signal(SIGTSTP, SIG_IGN);

    char input[MAX_INPUT];

    while (1) {

        check_jobs();

        interrupted = 0;
        printf("MiniShell> ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {

            if (interrupted) {
                clearerr(stdin);
                printf("\n");
                continue;
            }

            if (feof(stdin)) {
                printf("\n");
                break;
            }

            if (errno == EINTR) {
                clearerr(stdin);
                printf("\n");
                continue;
            }

            perror("fgets");
            clearerr(stdin);
            continue;
        }

        input[strcspn(input, "\n")] = '\0';

        if (strlen(input) == 0) {
            continue;
        }

        char original_command[MAX_INPUT];
        strncpy(original_command, input, MAX_INPUT - 1);
        original_command[MAX_INPUT - 1] = '\0';

        char *args[MAX_ARGS];
        int count = 0;

        char *token = strtok(input, " \t");

        while (token != NULL && count < MAX_ARGS - 1) {
            args[count++] = token;
            token = strtok(NULL, " \t");
        }

        args[count] = NULL;

        if (count == 0) {
            continue;
        }

        // Built-in: exit
        if (strcmp(args[0], "exit") == 0) {
            printf("Exiting MiniShell...\n");
            break;
        }

        // Built-in: cd
        if (strcmp(args[0], "cd") == 0) {
            if (args[1] == NULL) {
                fprintf(stderr, "Usage: cd directory\n");
            } else if (chdir(args[1]) != 0) {
                perror("cd");
            }
            continue;
        }

        // Built-in: pwd
        if (strcmp(args[0], "pwd") == 0) {
            char cwd[MAX_INPUT];

            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                printf("%s\n", cwd);
            } else {
                perror("pwd");
            }
            continue;
        }

        // Built-in: info
        if (strcmp(args[0], "info") == 0) {
            show_info();
            continue;
        }

        // Built-in: mypid
        if (strcmp(args[0], "mypid") == 0) {
            printf("MiniShell PID: %d\n", getpid());
            continue;
        }

        // Built-in: clear
        if (strcmp(args[0], "clear") == 0) {
            printf("\033[2J\033[H");
            fflush(stdout);
            continue;
        }

        // Built-in: jobs
        if (strcmp(args[0], "jobs") == 0) {
            show_jobs();
            continue;
        }

        // Built-in: kill PID
        if (strcmp(args[0], "kill") == 0) {
            if (args[1] == NULL) {
                printf("Usage: kill PID\n");
            } else {
                char *end;
                long value = strtol(args[1], &end, 10);

                if (*end != '\0' || value <= 0 ||
                    value > 2147483647L) {
                    printf("Invalid PID.\n");
                } else {
                    terminate_job((pid_t)value);
                }
            }
            continue;
        }

        // Background execution
        int background = 0;

        if (count > 0 && strcmp(args[count - 1], "&") == 0) {
            background = 1;
            args[--count] = NULL;
        }

        if (count == 0) {
            continue;
        }

        // Check for one pipe
        int pipe_pos = -1;

        for (int i = 0; i < count; i++) {
            if (strcmp(args[i], "|") == 0) {
                pipe_pos = i;
                break;
            }
        }

        if (pipe_pos != -1) {

            if (pipe_pos == 0 || pipe_pos == count - 1) {
                printf("Invalid pipe command.\n");
                continue;
            }

            if (pipe_pos != count - 1 &&
                pipe_pos + 1 < count) {
                int extra_pipe = 0;

                for (int i = pipe_pos + 1; i < count; i++) {
                    if (strcmp(args[i], "|") == 0) {
                        extra_pipe = 1;
                        break;
                    }
                }

                if (extra_pipe) {
                    printf("Only one pipe is supported.\n");
                    continue;
                }
            }

            args[pipe_pos] = NULL;
            char **left = args;
            char **right = &args[pipe_pos + 1];

            int fd[2];

            if (pipe(fd) == -1) {
                perror("pipe");
                continue;
            }

            pid_t pid1 = fork();

            if (pid1 == 0) {
                signal(SIGINT, SIG_DFL);
                signal(SIGTSTP, SIG_DFL);

                close(fd[0]);
                execute_command(left, STDIN_FILENO, fd[1]);
            }

            if (pid1 < 0) {
                perror("fork");
                close(fd[0]);
                close(fd[1]);
                continue;
            }

            pid_t pid2 = fork();

            if (pid2 == 0) {
                signal(SIGINT, SIG_DFL);
                signal(SIGTSTP, SIG_DFL);

                close(fd[1]);
                execute_command(right, fd[0], STDOUT_FILENO);
            }

            if (pid2 < 0) {
                perror("fork");
                kill(pid1, SIGTERM);
                close(fd[0]);
                close(fd[1]);
                waitpid(pid1, NULL, 0);
                continue;
            }

            close(fd[0]);
            close(fd[1]);

            if (background) {
                add_job(pid1, original_command);
                add_job(pid2, original_command);
                printf("[Background PIDs: %d, %d]\n", pid1, pid2);
            } else {
                int status;

                while (waitpid(pid1, &status, 0) == -1 &&
                       errno == EINTR) {
                }

                while (waitpid(pid2, &status, 0) == -1 &&
                       errno == EINTR) {
                }
            }

        } else {

            pid_t pid = fork();

            if (pid == 0) {
                signal(SIGINT, SIG_DFL);
                signal(SIGTSTP, SIG_DFL);

                execute_command(args, STDIN_FILENO, STDOUT_FILENO);
            }

            if (pid < 0) {
                perror("fork");
                continue;
            }

            if (background) {
                add_job(pid, original_command);
                printf("[Background PID: %d]\n", pid);
            } else {
                int status;

                while (waitpid(pid, &status, 0) == -1) {
                    if (errno == EINTR) {
                        continue;
                    }
                    perror("waitpid");
                    break;
                }
            }
        }
    }

    return 0;
}
