#include "cursor.h"
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>

cursor_t *new_cursor(int x_bound, int y_bound)
{
    if (x_bound < 1 || y_bound < 1)
    {
        return NULL;
    }

    cursor_t *new_cursor = calloc(1, sizeof(cursor_t));
    if (new_cursor == NULL)
    {
        return NULL;
        ;
    }
    new_cursor->x_bound = x_bound;
    new_cursor->y_bound = y_bound;
    new_cursor->x = 1;
    new_cursor->y = 1;

    return new_cursor;
}

int update_x(cursor_t *cursor, int new_x)
{
    if (new_x < 1 || new_x > cursor->x_bound)
    {
        return -1;
    }
    cursor->x = new_x;
    return 0;
}

int update_y(cursor_t *cursor, int new_y)
{
    if (new_y < 1 || new_y > cursor->y_bound)
    {
        return -1;
    }
    cursor->y = new_y;
    return 0;
}

void move_cursor(int x, int y)
{
    printf("\033[%d;%dH", x, y);
}

void slide_cursor(int x, int y)
{
    printf("\033[%dC\033[%dB", x, y);
}

cursor_t *new_bounded_cursor()
{
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    return new_cursor(w.ws_col, w.ws_row);
}