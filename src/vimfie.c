#include "vimfie.h"
#include <string.h>
#include <stdlib.h>

void free_vimfie(vimfie_t *vf)
{
    if (vf == NULL)
    {
        return;
    }

    if (vf->command != NULL)
    {
        free(vf->command);
    }
    if (vf->filename != NULL)
    {
        free(vf->filename);
    }

    if (vf->cursor != NULL)
    {
        free(vf->cursor);
    }
    if (vf->head != NULL)
    {
        iteratively_free_lines(vf->head);
    }
}

vimfie_t *new_vimfie(char *filename)
{
    vimfie_t *vf = calloc(1, sizeof(vimfie_t));
    if (vf == NULL)
    {
        return NULL;
    }

    if (filename != NULL)
    {
        char *new_filename = calloc(strlen(filename) + 1, sizeof(char));
        if (new_filename == NULL)
        {
            free_vimfie(vf);
            return NULL;
        }
        strncpy(new_filename, filename, strlen(filename));

        vf->filename = new_filename;
        FILE *file = fopen(filename, "r");
        if (file == NULL)
        {
            free_vimfie(vf);
            return NULL;
        }
        vf->head = parse_file_to_lines(file);
        if (vf->head == NULL)
        {
            free_vimfie(vf);
            return NULL;
        }
        fclose(file);
    }
    else
    {
        vf->filename = NULL;
        vf->head = empty_line();
    }

    vf->current = vf->head;

    vf->cursor = new_bounded_cursor();
    if (vf->cursor == NULL)
    {
        free_vimfie(vf);
        return NULL;
    }

    vf->command = calloc(CMD_BUFFER_SIZE, sizeof(char));
    if (vf->command == NULL)
    {
        free_vimfie(vf);
        return NULL;
    }

    return vf;
}

void clear_screen()
{
    printf("\033[2J");
    fflush(stdout);
}

void display_command(vimfie_t *vf)
{
    int x = vf->cursor->x;
    int y = vf->cursor->y;

    int cursor_y_level = vf->cursor->y_bound;
    update_x(vf->cursor, 1);
    update_y(vf->cursor, cursor_y_level);
    move_cursor(vf->cursor);
    fflush(stdout);

    for (int i = 0; i < vf->cursor->x_bound; i++)
    {
        printf(" ");
    }
    fflush(stdout);

    move_cursor(vf->cursor);
    printf("%s", vf->command);

    vf->cursor->x = x;
    vf->cursor->y = y;
    move_cursor(vf->cursor);
    fflush(stdout);
}

void clear_cmd_buffer(vimfie_t *vf)
{
    memset(vf->command, 0, CMD_BUFFER_SIZE);
}

void display_visible_contents(vimfie_t *vf)
{
    int x = vf->cursor->x;
    int y = vf->cursor->y;
    update_x(vf->cursor, 1);
    update_y(vf->cursor, 1);
    move_cursor(vf->cursor);

    for (int i = 0; i < vf->cursor->y_bound-1; i++)
    {
        printf("\033[31m~\033[0m\n");
        fflush(stdout);
    }

    update_x(vf->cursor, 1);
    update_y(vf->cursor, 1);
    move_cursor(vf->cursor);
    display_n_lines(vf->head, vf->cursor->y_bound-1);

    vf->cursor->x = x;
    vf->cursor->y = y;
    move_cursor(vf->cursor);
    display_command(vf);
    fflush(stdout);
}