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
int insert_str_line_data(line_data_t *ld, int index, char* str);
line_data_t *copy_line_data(line_data_t *ld);

typedef struct line_t
{
    line_data_t *data;
    struct line_t *prev;
    struct line_t *next;
} line_t;

line_t* empty_line();
void free_line(line_t* line);