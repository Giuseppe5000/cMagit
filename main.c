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

#include "tui.h"

/* Print git status */
void git_status(tui* t) {
    FILE *fp;

    /* Open the command for reading. */
    fp = popen("git status --v --show-stash", "r");
    if (fp == NULL) {
        printf("Failed to run command\n" );
        exit(1);
    }

    clear_screen(t);
    print_text(t, fp, 0, 0);

    /* close */
    pclose(fp);
}

int main(void) {
    tui t = {0};
    init_tui(&t);

    git_status(&t);

    char buffer[t.cols + 1];
    while(1) {
        char c = get_key_input(&t);

        switch(c) {
            case 'h':
                move_cursor(&t, t.cursor_x - 1, t.cursor_y);
                break;

            case 'j':
                move_cursor(&t, t.cursor_x, t.cursor_y + 1);
                break;

            case 'k':
                move_cursor(&t, t.cursor_x, t.cursor_y - 1);
                break;

            case 'l':
                move_cursor(&t, t.cursor_x + 1, t.cursor_y);
                break;

            case 'v':
            case 'V':
                printf("Visual mode");
                fflush(stdout);
                break;

            case 's':
                get_line(&t, buffer);
                /* Function that parses the string and calls git add if
                 * the cursor was pointing a file */

                int i = 0;
                char c = buffer[i];
                char* filename_pos = NULL;

                while (c != '\r') {

                    /* Using : for detect if the line has a git status file
                     * I can do much better.*/
                    if (c == ':') {
                        c = buffer[++i];

                        /* Skip whitespace until it reaches the start of the filename */
                        while (c == ' ') c = buffer[++i];

                        filename_pos = &(buffer[i]);
                    }

                    c = buffer[++i];
                }
                buffer[i] = '\0';

                if (filename_pos) {
                    /* Reusing buffer for compose git add command */
                    sprintf(buffer, "git add '%s'", filename_pos);
                    system(buffer);
                    git_status(&t);
                }

                break;

            case 'u':
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

    delete_tui(&t);

    return 0;
}
