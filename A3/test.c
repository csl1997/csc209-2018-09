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
  if (argc > 2) {
    fprintf(stderr, "Too many arguement\n");
    exit(1);
  }

  Node *head = NULL;
  char **filenames = init_filenames();
  char *listfile = "index";
  char *namefile = "filenames";


  read_list(listfile, namefile, &head, filenames);
  display_list(head, filenames);

  FreqRecord *freq = get_word(argv[1], head, filenames);

  print_freq_records(freq);
  return 0;
}
