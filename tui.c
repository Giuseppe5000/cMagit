#include "tui.h"

int init_tui(tui* t) {

    /* Init the tui struct */
    t->cursor_x = 1;
    t->cursor_y = 1;
    t->input_fd = STDIN_FILENO;
    t->output_fd = STDOUT_FILENO;

    /* Enable terminal raw mode */
    struct termios raw;

    if (!isatty(t->input_fd)) goto fatal;
    if (tcgetattr(t->input_fd,&(t->orig_termios)) == -1) goto fatal;

    raw = t->orig_termios;  /* modify the original mode */
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
    if (tcsetattr(t->input_fd,TCSAFLUSH,&raw) < 0) goto fatal;
    return 0;

fatal:
    errno = ENOTTY;
    return -1;
}


/* Disable terminal raw mode, restoring the initial term conf*/
void delete_tui(tui* t) {
    tcsetattr(t->input_fd,TCSAFLUSH,&(t->orig_termios));
}


void clear_screen(tui* t) {
    write(t->output_fd, "\e[1;1H\e[2J", 11);
}


void move_cursor(tui* t, int x, int y) {

    if (x > 0 && y > 0) {
        /* Update cursor on the struct */
        t->cursor_x = x;
        t->cursor_y = y;

        /* Update cursor on the screen*/
        printf("\x1b[%d;%dH", t->cursor_y, t->cursor_x);
        fflush(stdout);
    }

}


char get_key_input(tui* t) {
    int nread;
    char c, seq[3];
    while ((nread = read(t->input_fd,&c,1)) == 0);
    if (nread == -1) exit(1);

    return c;
}

void print_text(tui* t, FILE* fp, int x, int y) {

    /* Setting cursor to (x,y) for printing the text
     * and then we reset the cursor to the old value */
    int old_cursor_x = t->cursor_x;
    int old_cursor_y = t->cursor_y;
    move_cursor(t, x, y);


    char c = fgetc(fp);
    while (c != EOF) {

        /* Adding \r before each \n for going to the next line
         * in the right way
         */
        if (c == '\n') printf("\r");

        printf("%c", c);
        c = fgetc(fp);
    }

    move_cursor(t, old_cursor_x, old_cursor_y);
}

void get_line(tui* t, char* buffer) {
    fprintf(stderr, "UNIMPLEMENTED");
    exit(1);
}
