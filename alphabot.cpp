#include "alphabot.h"
#include <pigpiod_if2.h>
#include <math.h>
#include <iostream>
#include <unistd.h>


AlphaBot::AlphaBot() {
	pi = pigpio_start(NULL, NULL);
	if (pi < 0) {
		std::string msg = "Cannot connect to the pigpio daemon.\n";
		std::cerr << msg;
		exit(1);
	}
}

void AlphaBot::initPWM(int gpio, int pwm_frequency) {
        set_PWM_frequency(pi,gpio,pwm_frequency);
        int rr = get_PWM_real_range(pi, gpio);
        if ( ( rr > 255) && (rr < 20000) ) set_PWM_range(pi, gpio, rr);
        set_PWM_dutycycle(pi,gpio,0);
}

void AlphaBot::start(long _samplingInterval) {
        // motor control
        set_mode(pi,GPIO_ENA,PI_OUTPUT);
        set_mode(pi,GPIO_ENB,PI_OUTPUT);
        set_mode(pi,GPIO_IN1,PI_OUTPUT);
        set_mode(pi,GPIO_IN1,PI_OUTPUT);
        set_mode(pi,GPIO_IN2,PI_OUTPUT);
        set_mode(pi,GPIO_IN3,PI_OUTPUT);
        set_mode(pi,GPIO_IN4,PI_OUTPUT);

        initPWM(GPIO_ENA);
        initPWM(GPIO_ENB);

        // collision sensor
        set_mode(pi,GPIO_COLLISION_L,PI_INPUT);
        set_mode(pi,GPIO_COLLISION_R,PI_INPUT);

        // wheel speed
        leftWheelCallbackID = callback_ex(pi, GPIO_SPEED_L, EITHER_EDGE, encoderEvent, (void*)this);
        rightWheelCallbackID = callback_ex(pi, GPIO_SPEED_R, EITHER_EDGE, encoderEvent, (void*)this);

        // ADC
        set_mode(pi,GPIO_ADC_CS,PI_OUTPUT);
        gpio_write(pi,GPIO_ADC_CS,1);
        set_mode(pi,GPIO_ADC_ADDR,PI_OUTPUT);
        set_mode(pi,GPIO_ADC_IOCLK,PI_OUTPUT);
        set_mode(pi,GPIO_ADC_DOUT,PI_INPUT);

        // central processing
        samplingInterval = _samplingInterval;
	mainThread = new std::thread(worker,this);
}

void AlphaBot::stop() {
        running = false;
        callback_cancel(leftWheelCallbackID);
        callback_cancel(rightWheelCallbackID);
        if (nullptr != mainThread) {
                mainThread->join();
                mainThread = nullptr;
        }
        setLeftWheelSpeed(0);
        setRightWheelSpeed(0);
        pigpio_stop(pi);
}

void AlphaBot::encoderEvent(int pi, unsigned user_gpio, unsigned, uint32_t, void * userdata) {
        AlphaBot* alphabot = (AlphaBot*)userdata;
        if (pi != alphabot->pi) return;
        switch (user_gpio) {
                case GPIO_SPEED_L:
                        alphabot->leftWheelCounter++;
                        break;
                case GPIO_SPEED_R:
                        alphabot->rightWheelCounter++;
                        break;
        }
}

unsigned AlphaBot::readADC(unsigned channel) {
        gpio_write(pi,GPIO_ADC_CS,0);
	channel = channel << 4;
	for (unsigned i = 0; i < 4; i ++) {
		if(channel & 0x80)
			gpio_write(pi,GPIO_ADC_ADDR,1);
		else 
			gpio_write(pi,GPIO_ADC_ADDR,0);
		gpio_write(pi,GPIO_ADC_IOCLK,1);
		gpio_write(pi,GPIO_ADC_IOCLK,0);
		channel = channel << 1;
	}
	for (unsigned i = 0; i < 6;i ++) {
		gpio_write(pi,GPIO_ADC_IOCLK,1);
		gpio_write(pi,GPIO_ADC_IOCLK,0);
	}
 
        unsigned value = 0; 
	for (unsigned i = 0; i < 2; i ++) {
		gpio_write(pi,GPIO_ADC_IOCLK,1);
		value <<= 1;
		if (gpio_read(pi,GPIO_ADC_DOUT))
			value |= 0x1;
		gpio_write(pi,GPIO_ADC_IOCLK,0);
	} 

	for (unsigned i = 0; i < 8; i ++) {
		gpio_write(pi,GPIO_ADC_IOCLK,1);
		value <<= 1;
		if (gpio_read(pi,GPIO_ADC_DOUT))
			value |= 0x1;
		gpio_write(pi,GPIO_ADC_IOCLK,0);
	} 
        gpio_write(pi,GPIO_ADC_CS,1);

	return value; 
}
	

void AlphaBot::worker(AlphaBot* alphabot) {
        alphabot->running = true;
        bool errorReported = false;
        unsigned long t = alphabot->ms();
        // The Linux timer cannot be used because pigpiod is using it so we
        // need to resort to an imprecise sampling loop with jitter but
        // the average sampling rate is constant as the delay in the loop
        // is generated as an error signal between expected timestamp
        // and actual timestamp.
        while (alphabot->running) {
                // ADC
                alphabot->leftDistance = alphabot->readADC(alphabot->ADC_DIST_L);
                alphabot->rightDistance = alphabot->readADC(alphabot->ADC_DIST_R);
                alphabot->batteryLevel = alphabot->readADC(alphabot->ADC_VIN);
	
                alphabot->spinningCounter--;
                if (alphabot->spinningCounter < 1) {
                        alphabot->leftIsSpinning = alphabot->leftWheelCounter > 0;
                        alphabot->rightIsSpinning = alphabot->rightWheelCounter > 0;
                        alphabot->rightWheelCounter = 0;
                        alphabot->leftWheelCounter = 0;
                        alphabot->spinningCounter = 500 / alphabot->samplingInterval;
                }

                // callback
                if (nullptr != alphabot->stepCallback)
                        alphabot->stepCallback->step(*alphabot);

                // dynamic delay to achieve the sampling rate
                // timestamp we want to be
                t += alphabot->samplingInterval;
                // timestamp we are actually
                unsigned long actual = alphabot->ms();
                // delay needed to end up at the current t
                long d = t - actual;
                if (d > 0) {
                        usleep(d*1000);
                } else {
                        // requested sampling interval is too short
                        if (!errorReported) {
                                std::cerr << "Sampling rate is too high.";
                                errorReported = true;
                        }
                }
        }
}

void AlphaBot::setRightWheelSpeed(float speed) {
        if (speed < -1)
                speed = -1;
        if (speed > 1)
                speed = 1;
        rightWheelSpeed = speed;
        if (speed < 0) {
                gpio_write(pi,GPIO_IN3,1);
                gpio_write(pi,GPIO_IN4,0);
        } else {
                gpio_write(pi,GPIO_IN3,0);
                gpio_write(pi,GPIO_IN4,1);
        }
        float max = (float)get_PWM_range(pi,GPIO_ENB);
        set_PWM_dutycycle(pi,GPIO_ENB,(int)round(fabs(speed)*max));
}

void AlphaBot::setLeftWheelSpeed(float speed) {
        if (speed < -1)
                speed = -1;
        if (speed > 1)
                speed = 1;
        leftWheelSpeed = speed;
        if (speed < 0) {
                gpio_write(pi,GPIO_IN1,0);
                gpio_write(pi,GPIO_IN2,1);
        } else {
                gpio_write(pi,GPIO_IN1,1);
                gpio_write(pi,GPIO_IN2,0);
        }
        float max = (float)get_PWM_range(pi,GPIO_ENA);
        set_PWM_dutycycle(pi,GPIO_ENA,(int)round(fabs(speed)*max));
}


bool AlphaBot::getCollisionLeft() {
        return gpio_read(pi, GPIO_COLLISION_L) == 0; 
}

bool AlphaBot::getCollisionRight() {
        return gpio_read(pi, GPIO_COLLISION_R) == 0; 
}
