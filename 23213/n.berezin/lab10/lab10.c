#include <sys/types.h>

int main(int argc, char *argv[]){
    if (argc != 2){
        printf("Usage: %s <command>\n", argv[0]);
        return 1;
    }

    pid_t pid = fork();
    if (pid == -1){
        perror("Can't fork");
        return 1;
    } else if (pid == 0){
        perror("Can't exec");
        return 1;
    }

    return 0;
}
