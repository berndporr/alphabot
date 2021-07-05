/**
 * Copyright (c) 2021 by Bernd Porr
 * Apache License 2.0
 **/

#ifndef ALPHABOT_H
#define ALPHABOT_H

#include <thread>
#include <sys/time.h>
#include <CppTimer.h>

#define DEFAULT_SAMPLING_INTERVAL_MS 100 // ms

/**
 * Alphabot class which communicates with the Alphabot hardware
 */
class AlphaBot : public CppTimer
{

public:
        /**
        * Callback interface which is called at the specified sampling
        * rate. The method step needs to be implemented.
        */
        class StepCallback
        {
        public:
                /**
                * Called at the specified sampling rate with a reference
                * to the Alphabot instance which has all the getters for
                * the data and setters for the motor control.
                */
                virtual void step(AlphaBot &) = 0;
        };

public:
        /**
         * Starts the communication with the robot.
         * 
         * @param _samplingInterval Sampling interval in ms for the ADC data
         */
        void start(long _samplingInterval = DEFAULT_SAMPLING_INTERVAL_MS);

        /**
         * Stops the communication with the Alphabot
         */
        void stop();

        /**
         * Destroys the Alpha Bot object and stops any communcation
         */
        ~AlphaBot()
        {
                stop();
        }

        /** 
         * registers callback which signals new data
         * @param _stepcallback A pointer to the callback interface
         **/
        void registerStepCallback(StepCallback *_stepcallback)
        {
                stepCallback = _stepcallback;
        }

        /**
         * Set the Left Wheel Speed
         * @param speed between -1 and +1
         */
        void setLeftWheelSpeed(float speed);

        /**
         * Set the Right Wheel Speed
         * @param speed between -1 and +1
         */
        void setRightWheelSpeed(float speed);

        /**
         * Info if the left wheel is spinning.
         * @return true Wheel is spinning
         * @return false While is not spinning
         */
        bool getLeftWheelSpinning() { return leftIsSpinning; }

        /**
         * Info if the right wheel is spinning.
         * @return true Wheel is spinning
         * @return false While is not spinning
         */
        bool getRightWheelSpinning() { return rightIsSpinning; }

        /**
         * Get the Battery Level
         * @return float Battery level in Volt
         */
        float getBatteryLevel() { return (float)batteryLevel / ADCmax * ADCvref; }

        /**
         * Get the Collision Left (digital signal from the distance sensor)
         * @return true Obstacle detected
         * @return false No Obstacle
         */
        bool getCollisionLeft();

        /**
         * Get the Collision Right (digital signal from the distance sensor)
         * @return true Obstacle detected
         * @return false No Obstacle
         */
        bool getCollisionRight();

        /**
         * Get the Left Distance
         * @return float Value between 0 and 1 as relative distance.
         */
        float getLeftDistance() { return (float)leftDistance / ADCmax; }

        /**
         * Get the Right Distance
         * @return float Value between 0 and 1 as relative distance.
         */
        float getRightDistance() { return (float)rightDistance / ADCmax; }

        /**
         * Number of IR channels or anagloue input channels at the rear
         * of the robot.
         */
        static const unsigned nIR = 5;

        /**
         * Returns the IR channels as a reference to an array with
         * length nIR. This allows using C+11 style loops.
         * @return Reference to a const array with the 5 IR readings (0..1)
         */
        inline const float (&getIR())[nIR]
        {
                return ir;
        }

private:
        static const int GPIO_MOTORL = 27;
        static const int GPIO_MOTORR = 22;

	const float motor_rest_pulseduration = 1.5/20;
	const float speed2pwm = 0.1/20;

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

        virtual void initPWM(int gpio);

        static void encoderEventL(int user_gpio, int level, uint32_t tick, void *userdata);
        static void encoderEventR(int user_gpio, int level, uint32_t tick, void *userdata);

        unsigned readADC(unsigned ch);

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
        float ir[nIR] = {0, 0, 0, 0, 0};
        bool running = true;
};

#endif
