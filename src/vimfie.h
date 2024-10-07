#pragma once
#include "file_contents.h"
#include "cursor.h"
#define CMD_BUFFER_SIZE 100
#define NORMAL_MODE 0
#define INSERT_MODE 1

typedef struct
{
    line_t *head;
    line_t *current;
    cursor_t *cursor;
    char *filename;
    char *command;
    int mode;
    int top_line;
} vimfie_t;

void free_vimfie(vimfie_t *vf);
vimfie_t *new_vimfie(char *filename);
void clear_screen();
void display_command(vimfie_t *vf);
void clear_cmd_buffer(vimfie_t *vf);
void display_visible_contents(vimfie_t *vf);