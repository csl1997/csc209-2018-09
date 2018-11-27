#define MAXWORD 5

int main() {

    pid_t parentID, pid, fd[2];
    int i, j, k;
    int status = 0;
    char toWrite[5] = "abcd";

    pipe(fd);
    parentID = getpid();

    for (i = 0; i < 4; i++) {

        if (getpid() == parentID) {
            pid = fork();
        }

        if (pid < 0) {
            perror("fork");
            exit(1);

        } else if (pid == 0) {
            close(fd[1]);

            printf("Reading ...\n");
            char buf[MAXWORD];
            read(fd[0], buf, MAXWORD);

            printf("CHILD : %d\tread : %s\n", getpid(), buf);

            exit(0); // edited

        }

    }

    for (k = 0; k < 4; k++) {
        if (getpid() == parentID) {
            close(fd[0]);
            printf("Writing ...\n");
            write(fd[1], toWrite, MAXWORD);

            wait(&status); // edited
        }
    }

    return 0;
}
