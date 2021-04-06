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
		char tmp[256];
		sprintf(tmp,"L: %d, R: %d",alphabot.getCollisionLeft(),alphabot.getCollisionRight());
                mvaddstr(1,0,tmp);
                sprintf(tmp,"L speed = %d, R speed = %d",alphabot.getActualLeftWheelSpeed(),alphabot.getActualRightWheelSpeed());
                mvaddstr(2,0,tmp);
                int ch = getch();
                switch (ch)
                {
                case 27:
                        running = 0;
                        break;

                case 'l':
			mvaddstr(0, 0,"Setting left speed");
			refresh();
                        alphabot.setLeftWheelSpeed(0.5);
                        break;

                case 'r':
			mvaddstr(0,0,"Setting right speed");
			refresh();
                        alphabot.setRightWheelSpeed(0.5);
                        break;

                case ' ':
			mvaddstr(0,0,"Stopping           ");
			refresh();
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
