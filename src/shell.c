#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
}

int main() {
  int status = 1;
  do {
    printf("> ");
    char* user_input;
    char** user_args;
    char* last_arg;
    int argc = 0;

    user_input = read_input();
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
    }

    free(user_input);
    free(user_args);
  } while (status);

  return 0;
}