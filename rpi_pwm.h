#ifndef __RPIPWM
#define __RPIPWM

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<string>
#include <iostream>
#include<math.h>

class RPI_PWM {
public:

    /**
     * PWM class for the Raspberry PI
     * \param channel The GPIO channel which is 2 or 3 for the RPI5
     * \param frequency The PWM frequency
     * \param duty_cycle The initial duty cycle of the PWM (default 0)
     * \param chip The chip number (for RPI5 it's 2)
     **/
    void start(int channel, int frequency, int chip = 0) {
	chippath = "/sys/class/pwm/pwmchip" + std::to_string(chip);
	pwmpath = chippath + "/pwm" + std::to_string(channel);
	FILE* fp;
	std::string p = chippath+"/export";
	fp = fopen(p.c_str(), "w");
	if (NULL == fp) {
	    fprintf(stderr,"PWM device does not exist. Make sure to add 'dtoverlay=pwm-2chan' to /boot/firmware/config.txt.\n");
	    return;
	}
	fprintf(fp, "%d", channel);
	fclose(fp);
	usleep(100000);
	per = (int)1E9 / frequency;
	setPeriod(per);
	setDutyCyclePercent(0);
	enable();
    }

    void stop() {
	disable();
    }

    ~RPI_PWM() {
	disable();
    }

    /**
     * Sets the duty cycle.
     * \param v The duty cycle in percent.
     **/
    void setDutyCyclePercent(float v) {
	int dc = (int)round((float)per * (v / 100.0));
	setDutyCycleNanosecs(dc);
    }

    /**
     * Sets the duty cycle in nanoseconds
     **/
    void setDutyCycleNanosecs(int ns) {
	writeSYS(pwmpath+"/"+"duty_cycle", ns);
    }

private:
    void setPeriod(int ns) {
	writeSYS(pwmpath+"/"+"period", ns);
    }

    void enable() {
	writeSYS(pwmpath+"/"+"enable", 1);
    }
    
    void disable() {
	writeSYS(pwmpath+"/"+"enable", 0);
    }

    int per = 0;
    
    std::string chippath;
    std::string pwmpath;
    
    void writeSYS(std::string filename, int value) const {
	FILE* fp;
	fp = fopen(filename.c_str(), "w");
	if (NULL == fp) {
	    fprintf(stderr,"Cannot write to %s.\n",filename.c_str());
	    return;
	}
	fprintf(fp, "%d", value);
	fclose(fp);
    }
    
};

#endif
