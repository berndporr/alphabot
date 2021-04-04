#include "alphabot.h"
#include <ncurses.h>

    int running = 1;

int main(int, char **)
{
        AlphaBot alphabot;
        initscr();
        noecho();
        raw();
        clear();
        alphabot.start();
        while (running)
        {
                int ch = getch();
                if (ch == 27) {
                        running = 0;
                        break;
                }
        }
        alphabot.stop();
        endwin();
}