#include "alphabot.h"
#include <ncurses.h>
#include <iostream>

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
			std::cerr << "Setting left speed\n";
                        alphabot.setLeftWheelSpeed(0.5);
                        break;

                case 'r':
			std::cerr << "Setting right speed\n";
                        alphabot.setRightWheelSpeed(0.5);
                        break;

                case ' ':
			std::cerr << "Stopping\n";
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
