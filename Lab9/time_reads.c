/* The purpose of this program is to practice writing signal handling
 * functions and observing the behaviour of signals.
 */

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

/* Message to print in the signal handling function. */
#define MESSAGE "%ld reads were done in %ld seconds.\n"

/* Global variables to store number of read operations and seconds elapsed.
 */
long num_reads, seconds;


void handler (int code) {
  fprintf(stderr, MESSAGE, num_reads, seconds);
  exit(0);
}


/* The first command-line argument is the number of seconds to set a timer to run.
 * The second argument is the name of a binary file containing 100 ints.
 * Assume both of these arguments are correct.
 */

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: time_reads s filename\n");
        exit(1);
    }
    seconds = strtol(argv[1], NULL, 10);

    FILE *fp;
    if ((fp = fopen(argv[2], "r")) == NULL) {
      perror("fopen");
      exit(1);
    }

    struct sigaction sa;

    sa.sa_handler = handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGPROF, &sa, NULL);

    struct itimerval timer;
    timer.it_value.tv_sec = seconds;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = seconds;
    timer.it_interval.tv_usec = 0;

    if (setitimer(ITIMER_PROF, &timer, NULL) < 0) {
      perror("set timer");
      exit(1);
    }

    /* In an infinite loop, read an int from a random location in the file,
     * and print it to stderr.
     */
    int n;
    int r;
    for (;;) {

      r = random() % 100;

      if (fseek(fp, r * sizeof(int) , SEEK_SET)) {
        perror("seek");
        exit(1);
      }

      if (fread(&n, 1, sizeof(int), fp) <= 0) {
        perror("read");
        exit(1);
      }

      num_reads++;
      fprintf(stderr, "%d\n", n);


    }
    return 1; // something is wrong if we ever get here!
}
