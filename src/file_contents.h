#pragma once

typedef struct
{
    char *line_contents;
    int len;
    int capacity;
} line_data_t;

line_data_t *empty_line_data();
void free_line_data(line_data_t *ld);
line_data_t *str_to_line_data(char *str);
int resize_line_data(line_data_t *ld, int new_size);
int standard_upsize_line_data(line_data_t *ld, int minimum_new_size);
int insert_char_line_data(line_data_t *ld, int index, char c);

typedef struct file_content_t
{
    line_data_t *data;
    struct file_content_t *prev;
    struct file_content_t *next;
} file_content_t;
