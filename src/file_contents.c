#include "file_contents.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

int insert_str_line_data(line_data_t *ld, int index, char *str)
{
    if (ld == NULL || str == NULL)
    {
        return -1;
    }
    if (ld->line_contents == NULL)
    {
        return -1;
    }

    int len_str = strlen(str);
    for (int i = 0; i < len_str; i++)
    {
        int status = insert_char_line_data(ld, index + i, str[i]);
        if (status != 0)
        {
            return -1;
        }
    }

    return 0;
}

line_data_t *copy_line_data(line_data_t *ld)
{
    if (ld == NULL)
    {
        return NULL;
    }
    if (ld->line_contents == NULL)
    {
        return NULL;
    }

    line_data_t *new_ld = empty_line_data();
    if (new_ld == NULL)
    {
        return NULL;
    }
    int status = insert_str_line_data(new_ld, 0, ld->line_contents);
    if (status != 0)
    {
        free_line_data(new_ld);
        return NULL;
    }
    status = resize_line_data(new_ld, ld->capacity);
    if (status != 0)
    {
        free_line_data(new_ld);
        return NULL;
    }

    return new_ld;
}

line_t *empty_line()
{
    line_t *empty = calloc(1, sizeof(line_t));
    if (empty == NULL)
    {
        return NULL;
    }
    empty->data = empty_line_data();
    if (empty->data == NULL)
    {
        free(empty);
        return NULL;
    }
    empty->next = NULL;
    empty->prev = NULL;
    return empty;
}

void free_line(line_t *line)
{
    if (line == NULL)
    {
        return;
    }

    if (line->data != NULL)
    {
        free_line_data(line->data);
    }

    free(line);
}

line_t *str_to_line(char *str)
{
    line_t *line = empty_line();
    int status = insert_str_line_data(line->data, line->data->len, str);
    if (status != 0)
    {
        free_line(line);
        return NULL;
    }
    return line;
}

int insert_line(line_t *line, line_t *to_insert)
{
    if (line == NULL || to_insert == NULL)
    {
        return -1;
    }
    if (to_insert->next != NULL || to_insert->prev != NULL)
    {
        return -1;
    }

    if (line->next == NULL)
    {
        line->next = to_insert;
        to_insert->prev = line;
        return 0;
    }
    else
    {
        line_t *third = line->next;
        to_insert->next = third;
        to_insert->prev = line;
        third->prev = to_insert;
        line->next = to_insert;
        return 0;
    }
}

char **split_str(char *str, char c, int *return_size)
{
    if (str == NULL || return_size == NULL)
    {
        return NULL;
    }

    int len_of_str = strlen(str);
    if (len_of_str == 0)
    {
        return NULL;
    }

    int counter = 1;
    for (int i = 0; i < len_of_str; i++)
    {
        if (str[i] == c)
        {
            counter++;
        }
    }

    char **return_array = calloc(counter, sizeof(char *));
    counter = 0;
    int last = 0;
    for (int i = 0; i < len_of_str; i++)
    {
        if (str[i] == c || i == len_of_str - 1)
        {
            int current_str_len = i - last + 1;
            char *current_str = calloc(current_str_len + 1, sizeof(char));
            memcpy(current_str, str + last, current_str_len);
            return_array[counter] = current_str;
            last = i + 1;
            counter++;
        }
    }

    *return_size = counter;
    return return_array;
}

void iteratively_free_lines(line_t *head)
{
    if (head == NULL)
    {
        return;
    }
    if (head->next != NULL)
    {
        iteratively_free_lines(head->next);
    }
    free_line(head);
}

line_t *parse_str_to_lines(char *str)
{
    if (str == NULL)
    {
        return NULL;
    }

    int len_of_str = strlen(str);
    if (len_of_str == 0)
    {
        return NULL;
    }

    int return_size = 0;
    char **strs = split_str(str, '\n', &return_size);
    if (strs == NULL)
    {
        return NULL;
    }

    if (strs[0][strlen(strs[0]) - 1] == '\n')
    {
        strs[0][strlen(strs[0]) - 1] = '\0';
    }

    line_t *head = str_to_line(strs[0]);
    line_t *current = head;

    for (int i = 1; i < return_size; i++)
    {
        if (strs[i][strlen(strs[i]) - 1] == '\n')
        {
            strs[i][strlen(strs[i]) - 1] = '\0';
        }

        line_t *new_line = str_to_line(strs[i]);
        insert_line(current, new_line);
        current = new_line;
    }

    for (int i = 0; i < return_size; i++)
    {
        free(strs[i]);
    }
    free(strs);

    return head;
}

int write_line(line_t *line, int fd)
{
    if (line == NULL)
    {
        return -1;
    }
    write(fd, line->data->line_contents, line->data->len);
    return 0;
}

int write_lines_from_head(line_t *line, int fd)
{
    if (line == NULL)
    {
        return -1;
    }

    while (line != NULL)
    {
        write_line(line, fd);
        write(fd, "\n", 1);
        line = line->next;
    }

    return 0;
}

line_t *parse_file_to_lines(FILE *file)
{
    if (file == NULL)
    {
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    char *read_buffer = calloc(file_size + 1, sizeof(char));
    if (read_buffer == NULL)
    {
        return NULL;
    }

    fread(read_buffer, sizeof(char), file_size, file);

    line_t *head = parse_str_to_lines(read_buffer);

    free(read_buffer);
    return head;
}

int write_lines_to_file(line_t *line, FILE *file)
{
    if (line == NULL || file == NULL)
    {
        return -1;
    }

    while (line != NULL)
    {
        fwrite(line->data->line_contents, sizeof(char), line->data->len, file);
        fwrite("\n", sizeof(char), 1, file);
        line = line->next;
    }

    return 0;
}