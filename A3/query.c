#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include "freq_list.h"
#include "worker.h"

int main(int argc, char **argv) {
  char pathes[MAXWORKERS][PATHLENGTH];
  char path[PATHLENGTH];
  char dirname[PATHLENGTH] = ".";
  int count = 0;
  FreqRecord recs[MAXRECORDS];
  FreqRecord *rec = NULL;
  char word[MAXWORD];

  if (argc == 2) {
    strncpy(dirname, argv[1], PATHLENGTH);
    dirname[PATHLENGTH - 1] = '\0';
  } else if (argc > 2) {
    fprintf(stderr, "Too many arguements\n");
  }

  DIR *dirp;
  if ((dirp = opendir(dirname)) == NULL) {
      perror("opendir");
      exit(1);
  }

  struct dirent *dp;
  while ((dp = readdir(dirp)) != NULL) {
      if (strcmp(dp->d_name, ".") == 0 ||
          strcmp(dp->d_name, "..") == 0 ||
          strcmp(dp->d_name, ".svn") == 0 ||
          strcmp(dp->d_name, ".git") == 0) {
              continue;
      }

      path[0] = '\0';
      strncpy(path, dirname, PATHLENGTH);
      strncat(path, "/", PATHLENGTH-strlen(path));
      strncat(path, dp->d_name, PATHLENGTH-strlen(path));
      path[PATHLENGTH - 1] = '\0';

      struct stat sbuf;
      if (stat(path, &sbuf) == -1) {
          // This should only fail if we got the path wrong
          // or we don't have permissions on this entry.
          perror("stat");
          exit(1);
      }

      if (S_ISDIR(sbuf.st_mode)) {
          strcpy(pathes[count], path);
          count++;
      }
  }

  int fd_read[2];
  int fd_write[count][2];
  int status;
  int c = 0;
  pid_t parentID, pid;
  parentID = getpid();

  for (int i = 0; i < count; i++) {
    if (pipe(fd_read) == -1) {
      perror("pipe read");
      exit(1);
    }
    if (pipe(fd_write[i]) == -1) {
      perror("pipe write");
      exit(1);
    }

    if (getpid() == parentID) {
      pid = fork();
    }

    if (pid < 0) {
      perror("fork");
      exit(1);
    }


    if (pid == 0) { // child
        close(fd_read[1]);
        close(fd_write[i][0]);

        run_worker(pathes[i], fd_read[0], fd_write[i][1]);
        printf("child run worker\n");

        exit(0);

    } else { // parent
      close(fd_write[i][1]);
      close(fd_read[0]);

    }

  }

  // the parent
  if (getpid() == parentID) {
    while (read(STDIN_FILENO, word, MAXWORD) > 0) {
    printf("parent write\n");
    write(fd_read[1], word, MAXWORD);


    printf("wait for child\n");
    wait(&status);

    for (int f = 0; f < count; f++) {
      while (read(fd_write[f][0], rec, sizeof(FreqRecord)) > 0) {
        recs[c] = *rec;
        c++;

        printf("parent is reading\n");

        if (c == MAXRECORDS) break;
      }
    }
  }
}
  print_freq_records(recs);
}
