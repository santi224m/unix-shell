#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define STR_BUFFER_SIZE 1024
#define ARGS_BUFFER_SIZE 64
#define DELIM " "

char* read_input(void) {
  int buff_size = STR_BUFFER_SIZE;
  char* str_buff = malloc(sizeof(char) * buff_size);
  int i = 0;
  char c;

  if (!str_buff) {
    fprintf(stderr, "Error creating string buffer");
  }

  while (1) {
    c = getchar();
    if (c == '\n' || c == EOF) {
      str_buff[i] = '\0';
      return str_buff;
    }
    str_buff[i] = c;
    i++;

    // Allocate more memory if buffer is full
    if (i >= buff_size) {
      buff_size += STR_BUFFER_SIZE;
      str_buff = realloc(str_buff, sizeof(char) * buff_size);
      if (!str_buff) {
        fprintf(stderr, "Error when rellocating memory");
      }
    }
  }
}

char** parse_args(char* user_input) {
  char* tok;
  int buff_size = ARGS_BUFFER_SIZE;
  char** tokens = malloc(sizeof(char*) * buff_size);
  int pos = 0;

  tok = strtok(user_input, DELIM);
  while (tok != NULL) {
    tokens[pos] = tok;
    pos++;

    if (pos > buff_size) {
      buff_size += ARGS_BUFFER_SIZE;
      tokens = realloc(tokens, buff_size);
    }
    tok = strtok(NULL, DELIM);
  }

  return tokens;
}

void echo_args(char** user_args, int argc) {
  for (int i = 0; i < argc - 1; i++) {
    printf("%s\n", user_args[i]);
  }
}

void detect_spaces_and_pipes(char* user_input) {
  int i = 0;
  while (user_input[i]) {
    if (user_input[i] == ' ') {
      printf("SPACE\n");
    } else if (user_input[i] == '|') {
      printf("PIPE\n");
    }
    i++;
  }
}

void builtin_help(void) {
  printf("Welcome to this unix shell\n");
  printf("Commands:\n\n");
  printf("help\tShow this help menu\n");
  printf("exit\tExit the shell\n");
  printf("ECHO\tAppend ECHO to the end of the input to print each argument on a new line\n");
  printf("COMMAND > FILE\t Direct the outputs of COMMAND to FILE\n");
  printf("!!\t Rerun the last command\n");
}

void builtin_cd(char* new_dir) {
  int rc = chdir(new_dir);
  if (rc < 0) {
    fprintf(stderr, "Error changing directory\n");
  }
}

void builtin_mkdir(char* path) {
  int rc = mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  if (rc < 0) {
    fprintf(stderr, "Error creating directory\n");
  }
}

  void execute_command(char** user_args) {
    pid_t pid, wpid;
    int status;
    int redirection_pos = -1;
    char* redirect_path;

    for (int i = 0; user_args[i] != NULL; i++) {
      // Check for redirection sign: >
      if (strcmp(user_args[i], ">") == 0) {
        redirection_pos = i;
      }
    }

    int original_stdout_fd = dup(STDOUT_FILENO);
    if (redirection_pos > -1) {
      if (user_args[redirection_pos+1] == NULL) {
        fprintf(stderr, "Missing redirection file\n");
      }

      redirect_path = user_args[redirection_pos+1];
      int file_fd = open(redirect_path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
      user_args[redirection_pos+1] = NULL;
      user_args[redirection_pos] = NULL;
      if (dup2(file_fd, STDOUT_FILENO) == -1) {
          perror("Error redirecting stdout");
          close(file_fd);
          return;  // Exit the function
      }
      close(file_fd);
    }

    pid = fork();

    if (pid == 0) {
      // Child process
      if (execvp(user_args[0], user_args) == -1) {
        fprintf(stderr, "Error executing commands\n");
        exit(EXIT_FAILURE);
      }
    } else if (pid < 0) {
      fprintf(stderr, "Error when forking\n");
    } else {
      // Parent process
      do {
        wpid = waitpid(pid, &status, WUNTRACED);
      } while (!WIFEXITED(status) && !WEXITSTATUS(status));
      if (dup2(original_stdout_fd, STDOUT_FILENO) == -1) {
        perror("Error restoring stdout");
      }
      close(original_stdout_fd);
    }
  }

int main() {
  int status = 1;
  do {
    printf("> ");
    char* user_input;
    char** user_args;
    char* last_arg;
    int argc = 0;
    char* last_command;

    user_input = read_input();
    if (strcmp(user_input, "!!") == 0) {
      free(user_input);
      user_input = strdup(last_command);
    } else {
      free(last_command);
      last_command = strdup(user_input);
    }
    detect_spaces_and_pipes(user_input);
    user_args = parse_args(user_input);

    for (int i = 0; user_args[i] != NULL; i++) {
      last_arg = user_args[i];
      argc = i + 1;
    }

    if (strcmp(user_input, "exit") == 0) {
      status = 0;
    } else if (strcmp(last_arg, "ECHO") == 0) {
      echo_args(user_args, argc);
    } else if (strcmp(user_input, "help") == 0) {
      builtin_help();
    } else if (strcmp(user_args[0], "cd") == 0) {
      if (user_args[1] == NULL) {
        fprintf(stderr, "Missing directory\n");
      }
      builtin_cd(user_args[1]);
    } else if (strcmp(user_args[0], "mkdir") == 0) {
      if (user_args[1] == NULL) {
        fprintf(stderr, "Missing path for new directory\n");
      }
      builtin_mkdir(user_args[1]);
    } else {
      execute_command(user_args);
    }

    free(user_input);
    free(user_args);
  } while (status);

  return 0;
}