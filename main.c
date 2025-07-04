/*
 * Porting Magit (Emacs extension) in C.
 *
 * ---------
 *
 * I am a novice, so for some function (for example interacting with the VT100 escapes) I
 * have taken code from the kilo editor (https://github.com/antirez/kilo).
 *
 *
 *
 * By Giuseppe Tutino.
 *
 * */


#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

enum KEY_ACTION{
    CTRL_C = 3,
    TAB = 9,
    ESC = 27,
};

typedef struct {
    int x;
    int y;
} cursorPosition;

/* ================ RAW MODE ================ */

static struct termios orig_termios; /* In order to restore at exit.*/

void disableRawMode(int fd) {
    tcsetattr(fd,TCSAFLUSH,&orig_termios);
}

int enableRawMode(int fd) {
    struct termios raw;

    if (!isatty(STDIN_FILENO)) goto fatal;
    //atexit(editorAtExit);
    if (tcgetattr(fd,&orig_termios) == -1) goto fatal;

    raw = orig_termios;  /* modify the original mode */
    /* input modes: no break, no CR to NL, no parity check, no strip char,
     * no start/stop output control. */
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    /* output modes - disable post processing */
    raw.c_oflag &= ~(OPOST);
    /* control modes - set 8 bit chars */
    raw.c_cflag |= (CS8);
    /* local modes - choing off, canonical off, no extended functions,
     * no signal chars (^Z,^C) */
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    /* control chars - set return condition: min number of bytes and timer. */
    raw.c_cc[VMIN] = 0; /* Return each byte, or zero for timeout. */
    raw.c_cc[VTIME] = 1; /* 100 ms timeout (unit is tens of second). */

    /* put terminal in raw mode after flushing */
    if (tcsetattr(fd,TCSAFLUSH,&raw) < 0) goto fatal;
    return 0;

fatal:
    errno = ENOTTY;
    return -1;
}

/* ================================  */

void redraw_cursor(cursorPosition* cursor) {
    printf("\x1b[%d;%dH", cursor->y, cursor->x);
    fflush(stdout);
}

void set_cursor_pos(cursorPosition* cursor, int x, int y) {
    printf("\x1b[%d;%dH", y, x);
    fflush(stdout);
}

/* Clear terminal screen with escapes */
void clear_screen(cursorPosition* cursor) {
    write(STDOUT_FILENO, "\e[1;1H\e[2J", 11);
}

void processInput(int fd, cursorPosition* cursor) {
    int nread;
    char c, seq[3];
    while ((nread = read(fd,&c,1)) == 0);
    if (nread == -1) exit(1);

    switch(c) {
        case 'h':
            if (cursor->x > 1) cursor->x--;
            redraw_cursor(cursor);
            break;

        case 'j':
            cursor->y++;
            redraw_cursor(cursor);
            break;

        case 'k':
            if (cursor->y > 1) cursor->y--;
            redraw_cursor(cursor);
            break;

        case 'l':
            cursor->x++;
            redraw_cursor(cursor);
            break;

        case 'v':
        case 'V':
            printf("Visual mode");
            fflush(stdout);
            break;

        case 's':
            //clear_screen(&cursor);
            //draw_status(&cursor);
            printf("Stage file");
            fflush(stdout);
            break;

        case 'u':
            //clear_screen(&cursor);
            //draw_status(&cursor);
            printf("Unstage file");
            fflush(stdout);
            break;

        case TAB:
            printf("Open file fragments");
            fflush(stdout);
            break;

        case 'q':
        case CTRL_C:
        case ESC:
            printf("\r\nExiting...\n");
            exit(0);
    }
}

void draw_status(cursorPosition* cursor) {

    /* Setting cursor to (0,0) for drawing the status
     * and then we reset the cursor to the old value */
    cursorPosition old_position = *cursor;
    set_cursor_pos(cursor, 0, 0);


    FILE *fp;

    /* Open the command for reading. */
    fp = popen("git status --v --show-stash", "r");
    if (fp == NULL) {
        printf("Failed to run command\n" );
        exit(1);
    }

    char c = fgetc(fp);
    while (c != EOF) {

        /* Adding \r before each \n for going to the next line
         * in the right way
         */
        if (c == '\n') printf("\r");

        printf("%c", c);
        c = fgetc(fp);
    }

    /* close */
    pclose(fp);

    set_cursor_pos(cursor, old_position.x, old_position.y);
}

int main(void) {
    cursorPosition cursor = {.x = 1, .y = 1};

    enableRawMode(STDIN_FILENO);

    clear_screen(&cursor);
    draw_status(&cursor);

    while(1) {
        processInput(STDIN_FILENO, &cursor);
    }
    disableRawMode(STDIN_FILENO);

    return 0;
}
