/**
 * Copyright (c) 2021 by Bernd Porr
 * Apache License 2.0
 **/


#ifndef ALPHABOT_H
#define ALPHABOT_H

#include <thread>
#include "CppTimer.h"

#define DEFAULT_SAMPLING_INTERVAL_NS (100 * 1000 * 1000) // 100ms
#define WHEEL_TIMER_COUNT 10 // 1s

class AlphaBot : public CppTimer
{

public:
        class StepCallback
        {
        public:
                virtual void step(AlphaBot&) = 0;
        };

public:
        AlphaBot();
        void start(long _samplingInterval = DEFAULT_SAMPLING_INTERVAL_NS);
        void stop();
        void registerStepCallback(StepCallback *_stepcallback)
        {
                stepCallback = _stepcallback;
        }

        // set motor speed
        void setLeftWheelSpeed(float speed);
        void setRightWheelSpeed(float speed);

        int getActualLeftWheelSpeed() { return leftWheelActualSpeed; }
        int getActualRightWheelSpeed() { return rightWheelActualSpeed; }
        float getLeftDistance() { return (float)leftDistance / ADCmax; }
        float getRightDistance() { return (float)rightDistance / ADCmax; }
        float getBatteryLevel() { return (float)batteryLevel / ADCmax * ADCvref; }

        // get collision sensors
        bool getCollisionLeft();
        bool getCollisionRight();

private:
        static const int GPIO_ENA = 6;
        static const int GPIO_ENB = 26;
        static const int GPIO_IN1 = 12;
        static const int GPIO_IN2 = 13;
        static const int GPIO_IN3 = 20;
        static const int GPIO_IN4 = 21;

        // Digital collision sensors
        static const int GPIO_COLLISION_L = 19;
        static const int GPIO_COLLISION_R = 16;

        // Speed sensors
        static const int GPIO_SPEED_L = 8;
        static const int GPIO_SPEED_R = 7;

        // TLC1543
        static const int GPIO_ADC_IOCLK = 25;
        static const int GPIO_ADC_ADDR = 24;
        static const int GPIO_ADC_DOUT = 23;
        static const int GPIO_ADC_CS = 5;
        static const int ADC_IR1 = 0;
        static const int ADC_IR2 = 1;
        static const int ADC_IR3 = 2;
        static const int ADC_IR4 = 3;
        static const int ADC_IR5 = 4;
        static const int ADC_DIST_L = 8;
        static const int ADC_DIST_R = 9;
        static const int ADC_VIN = 10;
        static const unsigned ADCmax = 1023;
        static const unsigned ADCvref = 5;

        virtual void timerEvent();

        virtual void initPWM(int gpio, int pwm_frequency = 50);

        static void encoderEvent(int pi, unsigned user_gpio, unsigned level, uint32_t tick, void * userdata);

        unsigned readADC(unsigned ch);

        long samplingInterval;
        StepCallback *stepCallback = nullptr;
        float leftWheelSpeed = 0;
        float rightWheelSpeed = 0;
        int leftWeelCounter = 0;
        int rightWheelCounter = 0;
        int leftWheelActualSpeed = 0;
        int rightWheelActualSpeed = 0;
        int leftWheelCallbackID = 0;
        int rightWheelCallbackID = 0;
        int wheelTimerCounter = WHEEL_TIMER_COUNT;
        unsigned leftDistance = 0;
        unsigned rightDistance = 0;
        unsigned batteryLevel = 0;
	int pi = -1;
};

#endif
