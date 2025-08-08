#include "alphabot.h"
#include <math.h>
#include <iostream>
#include <unistd.h>

void AlphaBot::start(long _samplingInterval) {
    left_wheel_pwm.start(2,pwmfrequ);
    right_wheel_pwm.start(3,pwmfrequ);

        // ADC
        GPIO_ADC_CS.setValue(1);

        samplingInterval = _samplingInterval;

	startms(_samplingInterval);
}

void AlphaBot::stop() {
        setLeftWheelSpeed(0);
        setRightWheelSpeed(0);
}

unsigned AlphaBot::readADC(unsigned channel) {
    const unsigned us = 1;
    GPIO_ADC_CS.setValue(0);
    usleep(us);
    channel = channel << 4;
    for (unsigned i = 0; i < 4; i ++) {
	if(channel & 0x80)
	    GPIO_ADC_ADDR.setValue(1);
	else 
	    GPIO_ADC_ADDR.setValue(0);
	usleep(us);
	GPIO_ADC_IOCLK.setValue(1);
	usleep(us);
	GPIO_ADC_IOCLK.setValue(0);
	usleep(us);
	channel = channel << 1;
    }
    for (unsigned i = 0; i < 6;i ++) {
	usleep(us);
	GPIO_ADC_IOCLK.setValue(1);
	usleep(us);
	GPIO_ADC_IOCLK.setValue(0);
    }

    GPIO_ADC_CS.setValue(1);
    usleep(us);
    GPIO_ADC_CS.setValue(0);
	
    unsigned value = 0; 
    for (unsigned i = 0; i < 2; i ++) {
	GPIO_ADC_IOCLK.setValue(1);
	usleep(us);
	value <<= 1;
	usleep(us);
	if (GPIO_ADC_DOUT.getValue())
	    value |= 0x1;
	GPIO_ADC_IOCLK.setValue(0);
	usleep(us);
    } 

    for (unsigned i = 0; i < 8; i ++) {
	GPIO_ADC_IOCLK.setValue(1);
	usleep(us);
	value <<= 1;
	if (GPIO_ADC_DOUT.getValue())
	    value |= 0x1;
	GPIO_ADC_IOCLK.setValue(0);
	usleep(us);
    }
    usleep(us);
    GPIO_ADC_CS.setValue(1);
    return value; 
}

void AlphaBot::timerEvent() {
    // battery
    batteryLevel = (float)readADC(ADC_VIN) / ADCmax * ADCvref;
    // callback
    if (nullptr != batteryCallback)
	batteryCallback->hasBatteryVoltage(batteryLevel);
}

void AlphaBot::setRightWheelSpeed(float speed) {
	speed = -speed;
        if (speed < -1)
                speed = -1;
        if (speed > 1)
                speed = 1;
        rightWheelSpeed = speed;
	right_wheel_pwm.setDutyCycleNanosecs(speed2nanosec(speed));
}

void AlphaBot::setLeftWheelSpeed(float speed) {
        if (speed < -1)
                speed = -1;
        if (speed > 1)
                speed = 1;
        leftWheelSpeed = speed;
	left_wheel_pwm.setDutyCycleNanosecs(speed2nanosec(speed));
}
