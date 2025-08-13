/**
 * Copyright (c) 2025 by Bernd Porr
 * Apache License 2.0
 **/

#ifndef ALPHABOT_H
#define ALPHABOT_H

#include <thread>
#include <sys/time.h>
#include <gpiod.hpp>

#include "CppTimer.h"
#include "rpi_pwm.h"

#define DEFAULT_SAMPLING_INTERVAL_MS 500 // ms

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
    class BatteryCallback
        {
        public:
	    /**
	     * Called at the specified sampling rate with a reference
	     * to the Alphabot instance which has all the getters for
	     * the data and setters for the motor control.
	     */
	    virtual void hasBatteryVoltage(float volts) = 0;
        };

public:
    /**
     * Starts the callbacks about the battery level and enables the GPIO/PWM.
     * 
     * @param _samplingInterval Sampling interval in ms for the ADC data
     */
    void start(long _samplingInterval = DEFAULT_SAMPLING_INTERVAL_MS);
    
    /**
     * Stops the callbacks about the battery level and shuts down the GPIO/PWM.
     */
    void stop();
    
    /**
     * Destroys the Alpha Bot object and stops any communication
     */
    ~AlphaBot() {
	stop();
    }
    
    /** 
     * Registers callback which signals a battery voltage reading
     * @param _batterycallback A pointer to the callback interface
     **/
    void registerBatteryCallback(BatteryCallback *_batterycallback)
        {
	    batteryCallback = _batterycallback;
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

private:
    static constexpr int PWM_CHANNEL_MOTORL = 2;
    static constexpr int PWM_CHANNEL_MOTORR = 3;
    static constexpr int ADC_VIN = 10;

    static constexpr float pwmfrequ = 50; // Hz
    static constexpr float motor_rest_pulseduration_ns = 1.5 * 1E6;
    static constexpr float speed2pwm_ns = 0.1 * 1E6; // maps +-1 to +-0.1ms

    int speed2nanosec(float speed) const {
	return motor_rest_pulseduration_ns + speed * speed2pwm_ns;
    }
    
    // TLC1543
    ::gpiod::chip chip{"gpiochip0"};
    ::gpiod::line GPIO_ADC_IOCLK = chip.get_line(25);
    ::gpiod::line GPIO_ADC_ADDR = chip.get_line(24);
    ::gpiod::line GPIO_ADC_DOUT = chip.get_line(23);
    ::gpiod::line GPIO_ADC_CS = chip.get_line(5);
    static constexpr float ADCmax = 1023;
    static constexpr float ADCvref = 5;
    
    void timerEvent();
    
    unsigned readADC(unsigned ch);
    
    long samplingInterval;
    BatteryCallback *batteryCallback = nullptr;
    float leftWheelSpeed = 0;
    float rightWheelSpeed = 0;
    float batteryLevel = 0;
    bool running = false;
    
    RPI_PWM left_wheel_pwm;
    RPI_PWM right_wheel_pwm;
};

#endif
