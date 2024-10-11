#include <termios.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#include "vimfie.h"

int main(int argc, char **argv)
{
    vimfie_t *vf;
    if (argc >= 1)
    {
        vf = new_vimfie(argv[1]);
        if (vf == NULL)
        {
            printf("Couldn't find that file!!! Quitting!!!\n");
            return -1;
        }
    }
    else
    {
        vf = new_vimfie(NULL);
        if (vf == NULL)
        {
            printf("Vimfie failed to initialise. Quitting!!!\n");
            return -1;
        }
    }

    struct termios tty;

    tcgetattr(STDIN_FILENO, &tty);

    tty.c_lflag &= ~(ICANON | ECHO);

    tcsetattr(STDIN_FILENO, TCSANOW, &tty);

    clear_screen();
    move_cursor(vf->cursor);

    char c = '\0';
    do
    {
        display_visible_contents(vf);
        if (c == 'z')
        {
            break;
        }
        if (vf->mode == INSERT_MODE)
        {
            if (c == '\e')
            {
                vf->mode = NORMAL_MODE;
                clear_cmd_buffer(vf);
                update_x(vf->cursor, vf->cursor->x - 1);
                display_visible_contents(vf);
            }
            else if (c == '\r')
            {
                line_t *new_line = empty_line();
                insert_line(vf->current, new_line);
                update_x(vf->cursor, 1);
                update_y(vf->cursor, vf->cursor->y + 1);
                move_cursor(vf->cursor);
            }
            else
            {
                insert_char_line_data(vf->current->data, vf->cursor->x-1, c);
                update_x(vf->cursor, vf->cursor->x + 1);
                move_cursor(vf->cursor);
                display_visible_contents(vf);
            }
        }
        else if (vf->mode == NORMAL_MODE)
        {
            strncat(vf->command, &c, 1);
            if (strcmp(vf->command, "i") == 0)
            {
                vf->mode = INSERT_MODE;
                clear_cmd_buffer(vf);
                strcpy(vf->command, "--INSERT--");
                display_visible_contents(vf);
            }
            if (strcmp(vf->command, ":q\n") == 0)
            {
                break;
            }
            else if (c == '\n')
            {
                clear_cmd_buffer(vf);
                strcpy(vf->command, "Command not known!");
                display_visible_contents(vf);
                clear_cmd_buffer(vf);
            }
            else if (c == 'l')
            {
                update_x(vf->cursor, vf->cursor->x+1);
                clear_cmd_buffer(vf);
                display_visible_contents(vf);
            }
            else if (c == 'h')
            {
                update_x(vf->cursor, vf->cursor->x-1);
                clear_cmd_buffer(vf);
                display_visible_contents(vf);
            }
            else
            {
                display_visible_contents(vf);
            }
        }
    } while (read(STDIN_FILENO, &c, 1) == 1);

    free_vimfie(vf);
    clear_screen();
    move_cursor(vf->cursor);

    return 0;
}