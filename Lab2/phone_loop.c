#include <stdio.h>
#include <stdlib.h>

int main() {
  char word[11];
  int i;
  int err = 0;

  printf("Please type in 10 characters:\n");
  scanf("%s", word);

  printf("Please type some integers:\n");
  while (scanf("%d", &i) != EOF){

    if (i == -1){
      printf("%s\n", word);
    }
    else if (i > -1 && i < 10){
      printf("%c\n", word[i]);
    }
    else {
      printf("ERROR\n");
      err = 1;
    }
  }

  return err;
}
