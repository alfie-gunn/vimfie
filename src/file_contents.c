#include "file_contents.h"
#include <stdlib.h>
#include <string.h>

line_data_t *empty_line_data()
{
    line_data_t *new_line = calloc(1, sizeof(line_data_t));
    if (new_line != NULL)
    {
        new_line->line_contents = calloc(1, sizeof(char));
        if (new_line->line_contents != NULL)
        {
            new_line->capacity = 0;
            new_line->len = 0;
        }
        else
        {
            free(new_line);
            new_line = NULL;
        }
    }
    return new_line;
}

void free_line_data(line_data_t *ld)
{
    if (ld == NULL)
    {
        return;
    }
    if (ld->line_contents != NULL)
    {
        free(ld->line_contents);
    }
    free(ld);
}

line_data_t *str_to_line_data(char *str)
{
    if (str == NULL)
    {
        return NULL;
    }
    int len_of_str = strlen(str);
    line_data_t *ld = calloc(1, sizeof(line_data_t));
    if (ld == NULL)
    {
        return NULL;
    }
    ld->len = len_of_str;
    ld->capacity = len_of_str;
    ld->line_contents = calloc(len_of_str + 1, sizeof(char));
    if (ld->line_contents == NULL)
    {
        return NULL;
    }
    strncpy(ld->line_contents, str, len_of_str);
    return ld;
}

int resize_line_data(line_data_t *ld, int new_size)
{
    if (ld == NULL)
    {
        return -1;
    }
    if (new_size < ld->len)
    {
        return -1;
    }

    char *new_contents = calloc(new_size + 1, sizeof(char));
    if (new_contents == NULL)
    {
        return -1;
    }
    strncpy(new_contents, ld->line_contents, ld->len);
    free(ld->line_contents);
    ld->line_contents = new_contents;
    ld->capacity = new_size;
    return 0;
}

int standard_upsize_line_data(line_data_t *ld, int minimum_extra)
{
    if (ld == NULL || minimum_extra < 1)
    {
        return -1;
    }
    if (ld->capacity > minimum_extra)
    {
        return resize_line_data(ld, ld->capacity * 2);
    }
    else
    {
        return resize_line_data(ld, ld->capacity + minimum_extra);
    }
}

int insert_char_line_data(line_data_t *ld, int index, char c)
{
    if (ld == NULL || index < 0)
    {
        return -1;
    }
    if (index > ld->len)
    {
        return -1;
    }

    if (ld->len == ld->capacity)
    {
        int status = standard_upsize_line_data(ld, 1);
        if (status != 0)
        {
            return status;
        }
    }

    ld->len += 1;
    if (index == ld->len)
    {
        ld->line_contents[index] = c;
        return 0;
    }
    else
    {
        void *start_of_substring = ld->line_contents + index;
        int len_of_substring = strlen(start_of_substring);
        memmove(start_of_substring + 1, start_of_substring, len_of_substring);
        ld->line_contents[index] = c;
        return 0;
    }
}