/**
 * Copyright (c) 2021 by Bernd Porr
 * Apache License 2.0
 **/


#ifndef ALPHABOT_H
#define ALPHABOT_H

#include <thread>
#include <sys/time.h>
#include <CppTimer.h>

#define DEFAULT_SAMPLING_INTERVAL_MS 100 // 100ms

class AlphaBot : public CppTimer
{

public:
        class StepCallback
        {
        public:
                virtual void step(AlphaBot&) = 0;
        };

public:
        // starts the communication with the pigpiod
        void start(long _samplingInterval = DEFAULT_SAMPLING_INTERVAL_MS);

        // stops the communication with the pigpiod
        void stop();

        ~AlphaBot() {
                stop();
        }

        // registers callback which signals new data
        void registerStepCallback(StepCallback *_stepcallback)
        {
                stepCallback = _stepcallback;
        }

        // set motor speed
        void setLeftWheelSpeed(float speed);
        void setRightWheelSpeed(float speed);

        // returns true if the wheel is spining
        bool getLeftWheelSpinning() { return leftIsSpinning; }
        bool getRightWheelSpinning() { return rightIsSpinning; }

        // returns the battery level in Volt
        float getBatteryLevel() { return (float)batteryLevel / ADCmax * ADCvref; }

        // get collision sensors
        bool getCollisionLeft();
        bool getCollisionRight();

        // returns the distances
        float getLeftDistance() { return (float)leftDistance / ADCmax; }
        float getRightDistance() { return (float)rightDistance / ADCmax; }

        // number of IR channels
        static const unsigned nIR = 5;

        // returns the IR channels
        float (&getIR())[nIR] {
                return ir;
        }

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
        static const int GPIO_SPEED_L = 7;
        static const int GPIO_SPEED_R = 8;

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

        static void encoderEventL(int user_gpio, int level, uint32_t tick, void *userdata);
        static void encoderEventR(int user_gpio, int level, uint32_t tick, void *userdata);

        unsigned readADC(unsigned ch);

        static inline unsigned long ms() {
	        struct timeval tv;
	        gettimeofday(&tv, NULL);
	        return tv.tv_sec * 1000 + tv.tv_usec / 1000;
        }

        long samplingInterval;
        StepCallback *stepCallback = nullptr;
        float leftWheelSpeed = 0;
        float rightWheelSpeed = 0;
        int spinningCounter = 1;
        int leftWheelCounter = 0;
        int rightWheelCounter = 0;
        int leftWheelCallbackID = 0;
        int rightWheelCallbackID = 0;
        bool leftIsSpinning = false;
        bool rightIsSpinning = false;
        unsigned leftDistance = 0;
        unsigned rightDistance = 0;
        unsigned batteryLevel = 0;
        float ir[nIR] = {0,0,0,0,0};
        bool running = true;
        std::thread* mainThread = nullptr;
};

#endif
