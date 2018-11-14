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

  int fd_read[count][2];
  int fd_write[count][2];

  for (int i = 0; i < count; i++) {
    if (pipe(fd_read[i]) == -1) {
      perror("pipe read");
      exit(1);
    }
    if (pipe(fd_write[i]) == -1) {
      perror("pipe write");
      exit(1);
    }

    int r = fork();
    if (r < 0) {
      perror("fork");
      exit(1);
    }

    if (r > 0) { // parent
      close(fd_write[i][1]);
      close(fd_read[i][0]);

    } else { // child
      close(fd_read[i][1]);
      close(fd_write[i][0]);

      run_worker(pathes[i], fd_read[i][0], fd_write[i][1]);

      exit(0);
    }

  }

  // the parent
  int c = 0;
  while (read(STDIN_FILENO, word, MAXWORD) > 0) {
    for (int j = 0; j < count; j++) {
      write(fd_read[j][1], word, MAXWORD);
      while (read(fd_write[j][0], rec, sizeof(FreqRecord)) > 0) {
        recs[c] = *rec;
        c++;
        if (c == MAXRECORDS) break;
      }
    }
  }

  print_freq_records(recs);
}
