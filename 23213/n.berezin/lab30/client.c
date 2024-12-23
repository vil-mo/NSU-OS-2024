#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

const char *sock_path = "./.sock";

int main() {
    int sockfd;
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    strncpy(addr.sun_path, sock_path, sizeof(addr.sun_path) - 1);

    if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        return 1;
    }

    if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("connect");
        return 1;
    }

    char buf[1024];
    while (read(STDIN_FILENO, buf, sizeof(buf)) > 0) {
        write(sockfd, buf, strlen(buf));
    }

    return 0;
}
