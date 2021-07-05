#include "alphabot.h"
#include <pigpio.h>
#include <math.h>
#include <iostream>
#include <unistd.h>


void AlphaBot::initPWM(int gpio, int pwm_frequency) {
        gpioSetPWMfrequency(gpio,pwm_frequency);
        int rr = gpioGetPWMrealRange(gpio);
        if ( ( rr > 255) && (rr < 20000) ) gpioSetPWMrange(gpio, rr);
        setLeftWheelSpeed(0);
        setRightWheelSpeed(0);
}

void AlphaBot::start(long _samplingInterval) {
	int cfg = gpioCfgGetInternals();
	cfg |= PI_CFG_NOSIGHANDLER;
	gpioCfgSetInternals(cfg);
	int r = gpioInitialise();
	if (r < 0) {
		char msg[] = "Cannot init pigpio.";
		std::cerr << msg << "\n";
		throw msg;
	}

        initPWM(GPIO_MOTORL, 50);
        initPWM(GPIO_MOTORR, 50);

        // collision sensor
        gpioSetMode(GPIO_COLLISION_L,PI_INPUT);
        gpioSetMode(GPIO_COLLISION_R,PI_INPUT);

        // wheel speed
        leftWheelCallbackID = gpioSetAlertFuncEx(GPIO_SPEED_L, encoderEventL, (void*)this);
        rightWheelCallbackID = gpioSetAlertFuncEx(GPIO_SPEED_R, encoderEventR, (void*)this);

        // ADC
        gpioSetMode(GPIO_ADC_CS,PI_OUTPUT);
        gpioWrite(GPIO_ADC_CS,1);
        gpioSetMode(GPIO_ADC_ADDR,PI_OUTPUT);
        gpioSetMode(GPIO_ADC_IOCLK,PI_OUTPUT);
        gpioSetMode(GPIO_ADC_DOUT,PI_INPUT);

        // central processing
        samplingInterval = _samplingInterval;
	CppTimer::startms(samplingInterval);
}

void AlphaBot::stop() {
        CppTimer::stop();
	gpioSetAlertFuncEx(GPIO_SPEED_L, encoderEventL, NULL);
        gpioSetAlertFuncEx(GPIO_SPEED_R, encoderEventR, NULL);
        setLeftWheelSpeed(0);
        setRightWheelSpeed(0);
	gpioTerminate();
}

void AlphaBot::encoderEventL(int, int, uint32_t, void * userdata) {
        AlphaBot* alphabot = (AlphaBot*)userdata;
	alphabot->leftWheelCounter++;
}

void AlphaBot::encoderEventR(int, int, uint32_t, void * userdata) {
        AlphaBot* alphabot = (AlphaBot*)userdata;
	alphabot->rightWheelCounter++;
}

unsigned AlphaBot::readADC(unsigned channel) {
	unsigned us = 1;
        gpioWrite(GPIO_ADC_CS,0);
	usleep(us);
	channel = channel << 4;
	for (unsigned i = 0; i < 4; i ++) {
		if(channel & 0x80)
			gpioWrite(GPIO_ADC_ADDR,1);
		else 
			gpioWrite(GPIO_ADC_ADDR,0);
		usleep(us);
		gpioWrite(GPIO_ADC_IOCLK,1);
		usleep(us);
		gpioWrite(GPIO_ADC_IOCLK,0);
		usleep(us);
		channel = channel << 1;
	}
	for (unsigned i = 0; i < 6;i ++) {
		usleep(us);
		gpioWrite(GPIO_ADC_IOCLK,1);
		usleep(us);
		gpioWrite(GPIO_ADC_IOCLK,0);
	}

        gpioWrite(GPIO_ADC_CS,1);
	usleep(us);
	gpioWrite(GPIO_ADC_CS,0);
	
        unsigned value = 0; 
	for (unsigned i = 0; i < 2; i ++) {
		gpioWrite(GPIO_ADC_IOCLK,1);
		usleep(us);
		value <<= 1;
		usleep(us);
		if (gpioRead(GPIO_ADC_DOUT))
			value |= 0x1;
		gpioWrite(GPIO_ADC_IOCLK,0);
		usleep(us);
	} 

	for (unsigned i = 0; i < 8; i ++) {
		gpioWrite(GPIO_ADC_IOCLK,1);
		usleep(us);
		value <<= 1;
		if (gpioRead(GPIO_ADC_DOUT))
			value |= 0x1;
		gpioWrite(GPIO_ADC_IOCLK,0);
		usleep(us);
	}
	usleep(us);
        gpioWrite(GPIO_ADC_CS,1);

	return value; 
}
	

void AlphaBot::timerEvent() {
	// distance readings
	leftDistance = readADC(ADC_DIST_L);
	rightDistance = readADC(ADC_DIST_R);
	
	// battery
	batteryLevel = readADC(ADC_VIN);
	
	// IR sensors
	for(unsigned i=0;i<(nIR);i++) {
		ir[i] = (float)(readADC(i)) / ADCmax;
	}
	
	// wheel encoders
	spinningCounter--;
	if (spinningCounter < 1) {
		leftIsSpinning = leftWheelCounter > 0;
		rightIsSpinning = rightWheelCounter > 0;
		rightWheelCounter = 0;
		leftWheelCounter = 0;
		spinningCounter = 500 / samplingInterval;
	}
	
	// callback
	if (nullptr != stepCallback)
		stepCallback->step(*this);
	
}

void AlphaBot::setRightWheelSpeed(float speed) {
        if (speed < -1)
                speed = -1;
        if (speed > 1)
                speed = 1;
        rightWheelSpeed = speed;
        float max = (float)gpioGetPWMrange(GPIO_MOTORR);
	float rest = 1.5/20;
	speed = speed * rest /10;
        gpioPWM(GPIO_MOTORR,(int)round((rest+speed)*max));
}

void AlphaBot::setLeftWheelSpeed(float speed) {
        if (speed < -1)
                speed = -1;
        if (speed > 1)
                speed = 1;
        leftWheelSpeed = speed;
        float max = (float)gpioGetPWMrange(GPIO_MOTORL);
	float rest = 1.5/20;
	speed = speed * rest /10;	
        gpioPWM(GPIO_MOTORL,(int)round((rest+speed)*max));
}


bool AlphaBot::getCollisionLeft() {
        return gpioRead(GPIO_COLLISION_L) == 0; 
}

bool AlphaBot::getCollisionRight() {
        return gpioRead(GPIO_COLLISION_R) == 0; 
}
