#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]){
    // if (argc != 2) {
    //     if (argc > 0) {
    //         printf("Usage: %s <file>\n", argv[0]);
    //     } else {
    //         printf("Usage: lab9 <file>\n");
    //     }
    //     return 1;
    // }

    pid_t pid = fork();
    if (pid == -1){
        perror("Can't fork");
        return 1;
    } else if (pid == 0){
        execlp("#!/home/vilmo/Documents/lrn/a.out hm", "cat", "hahha", "-oeau", NULL);
        perror("Can't exec cat");
        return 1;
    } else {
        int status;
        wait(&status);
        printf("Child exited with status %d\n", WEXITSTATUS(status));
    }

    return 0;
}
