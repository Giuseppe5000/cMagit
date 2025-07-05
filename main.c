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

int main(void) {
    tui t = {0};

    init_tui(&t);

    clear_screen(&t);

    /* ======= Draw git status ======= */

    FILE *fp;

    /* Open the command for reading. */
    fp = popen("git status --v --show-stash", "r");
    if (fp == NULL) {
        printf("Failed to run command\n" );
        exit(1);
    }

    print_text(&t, fp, 0, 0);

    /* close */
    pclose(fp);

    /* ============== */


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
                printf("Stage file");
                fflush(stdout);
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
