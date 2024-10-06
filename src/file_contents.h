#pragma once

typedef struct
{
    char *line_contents;
    int len;
    int capacity;
} line_data_t;

typedef struct file_content_t
{
    line_data_t *data;
    struct file_content_t *prev;
    struct file_content_t *next;
} file_content_t;
