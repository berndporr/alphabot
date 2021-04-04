#ifndef ALPHABOT_H
#define ALPHABOT_H

#include <thread>
#include "CppTimer.h"

#define DEFAULT_SAMPLING_INTERVAL_NS (10 * 1000 * 1000) // 10ms

class AlphaBot : public CppTimer
{

public:
        class StepCallback
        {
        public:
                virtual void step() = 0;
        };

public:
        AlphaBot();
        void start(long _samplingInterval = DEFAULT_SAMPLING_INTERVAL_NS);
        void stop();
        void setStepCallback(StepCallback *_stepcallback)
        {
                stepCallback = _stepcallback;
        }

        void setLeftWheelSpeed(float speed);
        void setRightWheelSpeed(float speed);

private:
        static const int GPIO_ENA = 6;
        static const int GPIO_ENB = 26;
        static const int GPIO_IN1 = 12;
        static const int GPIO_IN2 = 13;
        static const int GPIO_IN3 = 20;
        static const int GPIO_IN4 = 21;

        static const int GPIO_COLLISION_L = 24; // GPIO=19;
        static const int GPIO_COLLISION_R = 27; // GPIO=16;

        virtual void timerEvent();

        long samplingInterval;
        StepCallback *stepCallback = nullptr;
        float leftWheelSpeed = 0;
        float rightWheelSpeed = 0;
};

#endif
