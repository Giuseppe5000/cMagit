/*
 *
 * Tui interface through VT100 escapes.
 *
 * */

#ifndef TUI_H
#define TUI_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <errno.h>
#include <unistd.h>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 1024

typedef struct {

    /* Screen status */
    int rows;
    int cols;
    char* screen; /* Screen characters */

    /* Cursor pos */
    int cursor_x;
    int cursor_y;

    /* Input/output descriptor */
    int input_fd;
    int output_fd;

    /* In order to restore terminal status at exit.*/
    struct termios orig_termios;

} tui;

enum KEY_ACTION{
    CTRL_C = 3,
    TAB = 9,
    ESC = 27,
};

/* Tui init setup
 *
 * returns -1 if it fails */
int init_tui(tui* t);

/* Tui delete setup, restoring initial terminal config */
void delete_tui(tui* t);

/* Clear the screen from any text */
void clear_screen(tui* t);

/* Move the cursor to the new specified position
 * making side effect on cursor_x and cursor_y */
void move_cursor(tui* t, int x, int y);

/* Get user keyboard input char.
 *
 * This is a blocking operation */
char get_key_input(tui* t);

/* Print some text (taken from a file pointer) to the screen,
 * at specified position (x,y) */
void print_text(tui* t, FILE* fp, int x, int y);

/* Get the characters on the cursor lines and puts in buffer.
 * Buffer size MUST be >= t->cols + 1 */
void get_line(tui* t, char* buffer);

#endif
