#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int main(int argc, char *argv[]) {
    char name[32];
    char previous[32];
    char most[32];
    int max_count = 0;
    int PPID;
    int count = 1;
    int count_P = 0;


    while (scanf("%s%*d%d%*[^\n]", name, &PPID) != EOF) {
      if (argc == 1) { // no command-line argument
        if (strcmp(previous, name) == 0) {
          count++;
        } else {
          if (count > max_count) {
            max_count = count;
            strcpy(most, name);
          }
          count = 1;
          strcpy(previous, name);
        }
        if (count > max_count) {
          max_count = count;
          strcpy(most, name);
        }

      } else if (argc == 2) { // 1 command-line argument
        int ID = strtol(argv[1], NULL, 10);
        if (strcmp(previous, name) == 0 && ID == PPID) {
          count_P++;
        } else {
          if (count_P > max_count) {
            max_count = count_P;
            strcpy(most, name);
          } else if (ID == PPID) {
            count_P = 1;
          } else {
            count_P = 0;
          }
          strcpy(previous, name);
        }
        if (count_P > max_count) {
          max_count = count_P;
          strcpy(most, name);
        }

      } else { // more than 1 command-line argument
        printf("USAGE: most_processes [ppid]\n");
        return 1;
      }
    }
    printf("%s %d\n", most, max_count);
    return 0;
}
