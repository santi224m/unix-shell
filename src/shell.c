#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STR_BUFFER_SIZE 1024;

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
    }
  }
}
int main() {
  int status = 1;
  do {
    printf("> ");
    char* str;
    str = read_input();
    while (*str != '\0') {
      printf("%c", *str);
      str++;
    }
    status = 0;
  } while (status);

  return 0;
}