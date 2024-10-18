#include <termios.h>
#include <stdio.h>
#include <unistd.h>

int main() {
    int fd = fileno(stdin);
    if (!isatty(fd)) {
        perror("Stdin is not a terminal");
        return 1;
    }

    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) {
        perror("Can't get terminal attributes");
        return 1;
    }
    
    struct termios saved_tty = tty;
    tty.c_lflag &= ~ICANON;
    tty.c_cc[VMIN] = 1;
    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("Can't set terminal attributes");
        return 1;
    }

    printf("What is the first letter of the alphabet?\n");
    int in = getchar();
    printf("\n");
    if (in == 'A' || in == 'a') {
        printf("Wow! Correct!\n");
    } else {
        printf("That's worng...\n");
    }
    
    if (tcsetattr(fd, TCSANOW, &saved_tty) != 0) {
        perror("Can't restore terminal attributes");
        return 1;
    }

    return 0;
}
