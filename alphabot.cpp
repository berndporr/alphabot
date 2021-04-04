#include "alphabot.h"
#include <pigpio.h>
#include <math.h>

#define MAX_PWM 100

AlphaBot::AlphaBot()
{
        if (gpioInitialise() < 0)
        {
                std::cerr << "Could not init gpio";
                exit(1);
        }
}

void AlphaBot::start(long _samplingInterval)
{

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
        if (speed < 0)
                speed = 0;
        if (speed > 1)
                speed = 1;
        leftWheelSpeed = speed;
}

void AlphaBot::setRightWheelSpeed(float speed)
{
        if (speed < 0)
                speed = 0;
        if (speed > 1)
                speed = 1;
        rightWheelSpeed = speed;
}
