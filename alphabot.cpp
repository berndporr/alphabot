#include "alphabot.h"
#include <pigpiod_if2.h>
#include <math.h>
#include <iostream>

#define MAX_PWM 256

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

        const int spi_flags = 0;
        adc = bb_spi_open(
                pi,
                GPIO_ADC_CS, GPIO_ADC_DOUT, GPIO_ADC_ADDR, GPIO_ADC_IOCLK,
                20000, spi_flags);

        // wheel speed
        leftWheelCallbackID = callback_ex(pi, GPIO_SPEED_L, RISING_EDGE, encoderEvent, (void*)this);
        rightWheelCallbackID = callback_ex(pi, GPIO_SPEED_R, RISING_EDGE, encoderEvent, (void*)this);

        // central processing
        samplingInterval = _samplingInterval;
        CppTimer::start(samplingInterval);
}

void AlphaBot::stop() {
        callback_cancel(leftWheelCallbackID);
        callback_cancel(rightWheelCallbackID);
        bb_spi_close(pi, GPIO_ADC_CS);
        CppTimer::stop();
        setLeftWheelSpeed(0);
        setRightWheelSpeed(0);
        pigpio_stop(pi);
}

void AlphaBot::encoderEvent(int pi, unsigned user_gpio, unsigned, uint32_t, void * userdata) {
        AlphaBot* alphabot = (AlphaBot*)userdata;
        if (pi != alphabot->pi) return;
        switch (user_gpio) {
                case GPIO_SPEED_L:
                        alphabot->leftWeelCounter++;
                        break;
                case GPIO_SPEED_R:
                        alphabot->rightWheelCounter++;
                        break;
        }
}

float AlphaBot::readADC(int ch) {
        char tx[2];
        char rx[2];
        tx[0] = (char)(ch << 4);
        bb_spi_xfer(pi, GPIO_ADC_CS, tx, rx, 2);
        unsigned r = ((unsigned)(rx[0]) << 8) | (unsigned)(rx[1]);
        return (float)r / 1024; 
}

void AlphaBot::timerEvent() {
        if (nullptr != stepCallback)
                stepCallback->step();

        // wheel speed
        wheelTimerCounter--;
        if (wheelTimerCounter < 1) {
                leftWheelActualSpeed = leftWeelCounter;
                leftWeelCounter = 0;
                rightWheelActualSpeed = rightWheelCounter;
                rightWheelCounter = 0;
                wheelTimerCounter = WHEEL_TIMER_COUNT;
        }

        // ADC
        leftDistance = readADC(ADC_DIST_L);
        rightDistance = readADC(ADC_DIST_R);
        batteryLevel = readADC(ADC_VIN);
}

void AlphaBot::setLeftWheelSpeed(float speed) {
        if (speed < -1)
                speed = -1;
        if (speed > 1)
                speed = 1;
        leftWheelSpeed = speed;
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

void AlphaBot::setRightWheelSpeed(float speed) {
        if (speed < -1)
                speed = -1;
        if (speed > 1)
                speed = 1;
        rightWheelSpeed = speed;
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
