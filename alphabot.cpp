#include "alphabot.h"
#include <pigpiod_if2.h>
#include <math.h>
#include <iostream>

#define MAX_PWM 256

AlphaBot::AlphaBot()
{
	pi = pigpio_start(NULL, NULL);
	if (pi < 0) {
		std::string msg = "Cannot connect to the pigpio daemon.\n";
		std::cerr << msg;
		exit(1);
	}
}

void AlphaBot::start(long _samplingInterval)
{
        set_mode(pi,GPIO_ENA,PI_OUTPUT);
        set_mode(pi,GPIO_ENB,PI_OUTPUT);
        set_mode(pi,GPIO_IN1,PI_OUTPUT);
        set_mode(pi,GPIO_IN1,PI_OUTPUT);
        set_mode(pi,GPIO_IN2,PI_OUTPUT);
        set_mode(pi,GPIO_IN3,PI_OUTPUT);
        set_mode(pi,GPIO_IN4,PI_OUTPUT);

        set_PWM_dutycycle(pi,GPIO_ENA,0);
        set_PWM_dutycycle(pi,GPIO_ENB,0);

        samplingInterval = _samplingInterval;
        CppTimer::start(samplingInterval);
}

void AlphaBot::stop()
{
        CppTimer::stop();
        setLeftWheelSpeed(0);
        setRightWheelSpeed(0);
        pigpio_stop(pi);
}

void AlphaBot::timerEvent()
{
        if (nullptr != stepCallback)
                stepCallback->step();
}

void AlphaBot::setLeftWheelSpeed(float speed)
{
        if (speed < -1)
                speed = -1;
        if (speed > 1)
                speed = 1;
        leftWheelSpeed = speed;
        if (speed < 0) {
                gpio_write(pi,GPIO_IN3,1);
                gpio_write(pi,GPIO_IN4,0);
        } else {
                gpio_write(pi,GPIO_IN3,0);
                gpio_write(pi,GPIO_IN4,1);
        }
        float max = (float)get_PWM_range(pi,GPIO_ENB);
        set_PWM_dutycycle(pi,GPIO_ENB,(int)round(fabs(speed)*max));
}

void AlphaBot::setRightWheelSpeed(float speed)
{
        if (speed < -1)
                speed = -1;
        if (speed > 1)
                speed = 1;
        rightWheelSpeed = speed;
        if (speed < 0) {
                gpio_write(pi,GPIO_IN1,0);
                gpio_write(pi,GPIO_IN2,1);
        } else {
                gpio_write(pi,GPIO_IN1,1);
                gpio_write(pi,GPIO_IN2,0);
        }
        float max = (float)get_PWM_range(pi,GPIO_ENA);
        set_PWM_dutycycle(pi,GPIO_ENA,(int)round(fabs(speed)*max));
}
