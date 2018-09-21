#include <stdio.h>
#include <stdlib.h>

int main() {
  char word[11];
  int i;

  printf("Please type in 10 characters:\n");
  scanf("%s", word);
  printf("Please type in an integer:\n");
  scanf("%d", &i);

  if (i == -1){
    printf("%s\n", word);
    return 0;
  }
  else if (i > -1 && i < 10){
    printf("%c\n", word[i]);
    return 0;
  }
  else {
    printf("ERROR\n");
    return 1;
  }
}
