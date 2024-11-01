#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define CTRL_D 4
#define CTRL_W 23

#define LINE_LENGTH 40

void fputs_stdin_exit_on_err(const char *s) {
    if (fputs(s, stdout) == EOF) {
        perror("Can't write");
        exit(1);
    }
}

void putchar_exit_on_err(int c) {
    int r = putchar(c);
    if (r == EOF) {
        perror("Can't write");
        exit(1);
    }
}

int main() {
    if (!isatty(STDIN_FILENO)) {
        fprintf(stderr, "Stdin is not a terminal\n");
        exit(1);
    }

    struct termios tty;
    if (tcgetattr(STDIN_FILENO, &tty) != 0) {
        perror("Can't get attributes of stdin");
        exit(1);
    }

    cc_t erase = tty.c_cc[VERASE];
    cc_t kill = tty.c_cc[VKILL];

    struct termios saved_tty = tty;
    tty.c_lflag &= ~(ISIG | ICANON | ECHO);
    tty.c_cc[VMIN] = 1;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) != 0) {
        perror("Can't set terminal attributes");
        exit(1);
    }

    // SAFETY: should be 0 <= pos <= LINE_LENGTH so it is always in-bounds to index buf
    // on change should comply with buf safety
    int pos = 0;
    // SAFETY: it is always safe to access `buf[x]` where `0 <= x < pos` symbols, and only those symbols
    char buf[LINE_LENGTH];

    while (1) {
        int ic = getchar();
        if (ic == EOF) {
            if (ferror(stdin)) {
                perror("Read error");
            } else {
                fprintf(stderr, "Unexpected EOF\n");
            }
            exit(1);
        }

        char c = (char)ic;

        if (c == erase) {
            if (pos > 0) {
                fputs_stdin_exit_on_err("\b\x1B[0K");
                pos--;
            }
        } else if (c == kill) {
            fputs_stdin_exit_on_err("\r\x1B[2K");
            pos = 0;
        } else if (c == CTRL_W) {
            int word_met = 0;
            while (pos > 0) {
                if (buf[pos - 1] != ' ') {
                    word_met = 1;
                } else if (word_met) {
                    break;
                }
                pos--;
                putchar_exit_on_err('\b');
            }
            fputs_stdin_exit_on_err("\x1B[0K");
        } else if (c == CTRL_D) {
            if (pos == 0) {
                break;
            }
        } else if (c == '\n') {
            pos = 0;
            putchar_exit_on_err('\n');
        } else if (32 <= c && c < 128) {
            pos++;
            if (pos > LINE_LENGTH) {
                int last_word_pos = LINE_LENGTH;
                pos = 0;
                
                while (last_word_pos > 0 && buf[last_word_pos - 1] != ' ') {
                    last_word_pos--;
                }

                if (last_word_pos != 0) {
                    for (int i = last_word_pos; i < LINE_LENGTH; i++) {
                        putchar_exit_on_err('\b');
                    }
                    fputs_stdin_exit_on_err("\x1B[0K\n");

                    // LINE_LENGTH - last_word_pos > 0 => pos will be < LINE_LENGTH
                    for (; last_word_pos < LINE_LENGTH; last_word_pos++) {
                        char current = buf[last_word_pos];
                        buf[pos] = current;
                        pos++;
                        putchar_exit_on_err(current);
                    }
                } else {
                    putchar_exit_on_err('\n');
                }
            } else {
                buf[pos - 1] = c;
                putchar_exit_on_err(c);
            }
        } else {
            putchar_exit_on_err('\a');
        }
    }


    if (tcsetattr(STDIN_FILENO, TCSANOW, &saved_tty) != 0) {
        perror("Can't restore terminal attributes");
        exit(1);
    }

    exit(0);
}
