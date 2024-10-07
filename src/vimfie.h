#pragma once
#include "file_contents.h"
#include "cursor.h"

typedef struct
{
    line_t *contents;
    cursor_t *cursor;
    char* command;
} vimfie_t;