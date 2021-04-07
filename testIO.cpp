#include "alphabot.h"
#include <ncurses.h>
#include <iostream>

int running = 1;

// callback every 100ms
class DisplaySensorCallback : public AlphaBot::StepCallback {
public:
	virtual void step(AlphaBot& alphabot) {
		char tmp[256];
		sprintf(tmp,"L: %d, R: %d     ",alphabot.getCollisionLeft(),alphabot.getCollisionRight());
                mvaddstr(1,0,tmp);
                sprintf(tmp,
			"L speed = %d, R speed = %d    ",
			alphabot.getActualLeftWheelSpeed(),
			alphabot.getActualRightWheelSpeed());
                mvaddstr(2,0,tmp);
                sprintf(tmp,
			"ADC_L = %f, ADC_R = %f, Power = %f    ",
			alphabot.getLeftDistance(),
			alphabot.getRightDistance(),
			alphabot.getBatteryLevel());
                mvaddstr(3,0,tmp);
		refresh();
	}
};

int main(int, char **)
{
        AlphaBot alphabot;
	DisplaySensorCallback displaySensorCallback;
	alphabot.registerStepCallback(&displaySensorCallback);
        initscr();
        noecho();
        clear();
        alphabot.start();
        while (running)
        {
		// blocking so that the main program sleeps here
                int ch = getchar();
                switch (ch)
                {
                case 27:
                        running = 0;
                        break;

                case 'l':
			mvaddstr(0, 0,"Setting left speed  ");
			refresh();
                        alphabot.setLeftWheelSpeed(0.5);
                        break;

                case 'r':
			mvaddstr(0,0,"Setting right speed  ");
			refresh();
                        alphabot.setRightWheelSpeed(0.5);
                        break;

                case ' ':
			mvaddstr(0,0,"Stopping             ");
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
