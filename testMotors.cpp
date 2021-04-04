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
                switch (ch)
                {
                case 27:
                        running = 0;
                        break;

                case 'l':
                        alphabot.setRightWheelSpeed(0.5);
                        break;

                case 'r':
                        alphabot.setRightWheelSpeed(0.5);
                        break;

                case ' ':
                        alphabot.setLeftWheelSpeed(0);
                        alphabot.setRightWheelSpeed(0);
                        break;

                default:
                        break;
                }
        }
        alphabot.stop();
        endwin();
}