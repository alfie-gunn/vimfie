#pragma once
#include "file_contents.h"
#include "pointer.h"

typedef struct
{
    file_content_t *contents;
    pointer_t *cursor;
    char* command;
} vimfie_t;