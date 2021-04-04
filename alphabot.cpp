#include "alphabot.h"
#include <pigpio.h>
#include <math.h>
#include <iostream>

#define MAX_PWM 256

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
        gpioSetMode(GPIO_ENA,PI_OUTPUT);
        gpioSetMode(GPIO_ENB,PI_OUTPUT);
        gpioSetMode(GPIO_IN1,PI_OUTPUT);
        gpioSetMode(GPIO_IN1,PI_OUTPUT);
        gpioSetMode(GPIO_IN2,PI_OUTPUT);
        gpioSetMode(GPIO_IN3,PI_OUTPUT);
        gpioSetMode(GPIO_IN4,PI_OUTPUT);

        gpioPWM(GPIO_ENA,0);
        gpioPWM(GPIO_ENB,0);

        samplingInterval = _samplingInterval;
        CppTimer::start(samplingInterval);
}

void AlphaBot::stop()
{
        CppTimer::stop();
        setLeftWheelSpeed(0);
        setRightWheelSpeed(0);
        gpioTerminate();
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
                gpioWrite(GPIO_IN3,1);
                gpioWrite(GPIO_IN4,0);
        } else {
                gpioWrite(GPIO_IN3,0);
                gpioWrite(GPIO_IN4,1);
        }
        float max = (float)gpioGetPWMrange(GPIO_ENB);
        gpioPWM(GPIO_ENB,(int)round(fabs(speed)*max));
}

void AlphaBot::setRightWheelSpeed(float speed)
{
        if (speed < -1)
                speed = -1;
        if (speed > 1)
                speed = 1;
        rightWheelSpeed = speed;
        if (speed < 0) {
                gpioWrite(GPIO_IN1,0);
                gpioWrite(GPIO_IN2,1);
        } else {
                gpioWrite(GPIO_IN1,1);
                gpioWrite(GPIO_IN2,0);
        }
        float max = (float)gpioGetPWMrange(GPIO_ENA);
        gpioPWM(GPIO_ENA,(int)round(fabs(speed)*max));
}
