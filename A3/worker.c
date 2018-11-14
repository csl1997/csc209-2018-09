#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>

#include "freq_list.h"
#include "worker.h"

/* Complete this function for Task 1. Including fixing this comment.
Return an array of FreqRecord that contains the frequency of the word that
appears in the correspond file.
*/
FreqRecord *get_word(char *word, Node *head, char **file_names) {
  Node *curr = head;
  int file_num = 0, count = 0;
  FreqRecord *rec;


  while (curr != NULL) {
    // find the word in the link list
    if (strncmp(curr->word, word, strlen(curr->word)) == 0) {

      // find the valid records number
      for (int j = 0; j < MAXFILES; j++) {
        if (curr->freq[j] > 0) {
          file_num++;
        }
      }

      if ((rec = malloc(sizeof(FreqRecord) * (file_num + 1))) == NULL) {
          perror("create_FreqRecord");
          exit(1);
      }
      for (int i = 0; i < MAXFILES; i++) {
        if (curr->freq[i] > 0) {
          rec[count].freq = curr->freq[i];
          strcpy(rec[count].filename, file_names[i]);
          count++;
        }
      }
      // last element of the array of FreqRecord
      rec[file_num].freq = 0;
      strcpy(rec[file_num].filename, "");
      break;
    }
    curr = curr->next;
  }

  // cannot find the word
  if (curr == NULL) {
    if ((rec = malloc(sizeof(FreqRecord))) == NULL) {
        perror("create_FreqRecord");
        exit(1);
    }
    rec[0].freq = 0;
    strcpy(rec[0].filename, "");
  }

  return rec;
}

/* Print to standard output the frequency records for a word.
* Use this for your own testing and also for query.c
*/
void print_freq_records(FreqRecord *frp) {
    int i = 0;

    while (frp != NULL && frp[i].freq != 0) {
        printf("%d    %s\n", frp[i].freq, frp[i].filename);
        i++;
    }

}

/* Complete this function for Task 2 including writing a better comment.
Read word from file descriptor in , find the frequency of the word and
write the data to file descriptor out.
*/
void run_worker(char *dirname, int in, int out) {
  Node *head = NULL;
  char **filenames = init_filenames();
  char listfile[PATHLENGTH];
  char namefile[PATHLENGTH];
  char word[MAXWORD];
  FreqRecord *rec;

  FreqRecord final;
  final.freq = 0;
  strcpy(final.filename, "");

  strncpy(listfile, dirname, PATHLENGTH);
  strncat(listfile, "/index", PATHLENGTH - strlen(listfile));
  strncpy(namefile, dirname, PATHLENGTH);
  strncat(namefile, "/filenames", PATHLENGTH - strlen(namefile));


  read_list(listfile, namefile, &head, filenames);


  while (read(in, word, MAXWORD) > 0) {
    rec = get_word(word, head, filenames);

    int i = 0;
    while (rec != NULL && rec[i].freq != 0) {
        if (write(out, &(rec[i]), sizeof(FreqRecord)) == -1) {
          perror("write");
        }
        i++;
    }
    if (write(out, &final, sizeof(FreqRecord)) == -1) {
      perror("write");
    }

  }

  return;
}
