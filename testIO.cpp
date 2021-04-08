#include "alphabot.h"
#include <ncurses.h>
#include <iostream>

int running = 1;

// callback every 100ms
class DisplaySensorCallback : public AlphaBot::StepCallback
{
public:
	virtual void step(AlphaBot &alphabot)
	{
		char tmp[256];
		sprintf(tmp, "Coll L = %d, Coll R = %d       ", alphabot.getCollisionLeft(), alphabot.getCollisionRight());
		mvaddstr(1, 0, tmp);
		sprintf(tmp,
				"Spinning L = %d, Spinning R = %d    ",
				alphabot.getLeftWheelSpinning(),
				alphabot.getRightWheelSpinning());
		mvaddstr(2, 0, tmp);
		sprintf(tmp,
				"Dist L = %1.4f, Dist R = %1.4f, Power = %1.1f Volt    ",
				alphabot.getLeftDistance(),
				alphabot.getRightDistance(),
				alphabot.getBatteryLevel());
		mvaddstr(3, 0, tmp);
		for(unsigned i = 0; i < alphabot.nIR; i++) {
			sprintf(tmp,"IR%d = %1.4f  ",i,alphabot.getIR()[i]);
			mvaddstr(4,i*15,tmp);
		}
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
	mvaddstr(0, 0, "l)eft wheel, r)ight wheel, f)orward, b)ackward, SPACE=stop, ESC=end");
	refresh();
	alphabot.start();
	float l = 0;
	float r = 0;
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
			l = l + 0.1f;
			alphabot.setLeftWheelSpeed(l);
			char tmp1[256];
			sprintf(tmp1, "Increasing right speed to %f         ", l);
			mvaddstr(6, 0, tmp1);
			refresh();
			break;

		case 'r':
			r = r + 0.1f;
			alphabot.setRightWheelSpeed(r);
			char tmp2[256];
			sprintf(tmp2, "Increasing right speed to %f         ", r);
			mvaddstr(6, 0, tmp2);
			refresh();
			break;

		case 'b':
			l = l - 0.05f;
			r = r - 0.05f;
			alphabot.setLeftWheelSpeed(l);
			alphabot.setRightWheelSpeed(r);
			mvaddstr(6, 0, "Backwards                            ");
			refresh();
			break;

		case 'f':
			l = l + 0.05f;
			r = r + 0.05f;
			alphabot.setLeftWheelSpeed(l);
			alphabot.setRightWheelSpeed(r);
			mvaddstr(6, 0, "Forwards                             ");
			refresh();
			break;

		case ' ':
			l = 0;
			r = 0;
			alphabot.setLeftWheelSpeed(l);
			alphabot.setRightWheelSpeed(r);
			mvaddstr(6, 0, "Stopping                            ");
			refresh();
			break;

		default:
			break;
		}
	}
	alphabot.stop();
	endwin();
}
