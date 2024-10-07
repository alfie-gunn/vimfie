#pragma once

typedef struct
{
    int x;
    int y;
    int x_bound;
    int y_bound;
} cursor_t;

cursor_t *new_cursor(int x_bound, int y_bound);
int update_x(cursor_t *cursor, int new_x);
int update_y(cursor_t *cursor, int new_y);
void move_cursor(cursor_t *cursor);
cursor_t *new_bounded_cursor();