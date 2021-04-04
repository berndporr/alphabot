#include "alphabot.h"
#include <wiringPi.h>
#include <softPwm.h>
#include <math.h>

#define MAX_PWM 100

AlphaBot::AlphaBot()
{
        if (wiringPiSetup() == -1)
        {
                throw "Could not set up wiring Pi. Might need to run as sudo.";
        }
}

void AlphaBot::start(long _samplingInterval)
{
        softPwmCreate(GPIO_WHEEL_L, 0, MAX_PWM);
        softPwmCreate(GPIO_WHEEL_R, 0, MAX_PWM);

        samplingInterval = _samplingInterval;
        CppTimer::start(samplingInterval);
}

void AlphaBot::stop()
{
        CppTimer::stop();
        setLeftWheelSpeed(0);
        setRightWheelSpeed(0);
}

void AlphaBot::timerEvent()
{
        if (nullptr != stepCallback)
                stepCallback->step();
}

void AlphaBot::setLeftWheelSpeed(float speed)
{
        if (speed < 0) speed = 0;
        if (speed > 1) speed = 1;
        leftWheelSpeed = speed;
        softPwmWrite(GPIO_WHEEL_L, (int)round(speed * MAX_PWM));
}

void AlphaBot::setRightWheelSpeed(float speed)
{
        if (speed < 0) speed = 0;
        if (speed > 1) speed = 1;
        rightWheelSpeed = speed;
        softPwmWrite(GPIO_WHEEL_R, (int)round(speed * MAX_PWM));
}
