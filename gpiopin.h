#ifndef __GPIOPIN_H
#define __GPIOPIN_H

/*
 * Copyright (c) 2025  Bernd Porr <mail@berndporr.me.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation. See the file LICENSE.
 */

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <thread>
#include <gpiod.h>
#include <vector>
#include <string>

// enable debug messages and error messages to stderr
#ifndef NDEBUG
#define DEBUG
#endif

#define ISR_TIMEOUT 1 // sec

class GPIOPin {

public:
    enum PinFunction {INPUT, OUTPUT, EVENT};
    
    /**
     * \param chipNo GPIO Chip number. It's usually 0.
     * \param pinNo GPIO Pin number.
     **/
    GPIOPin(int pinNo,
	    PinFunction func,
	    int ChipNo = 0);
    
    /**
     * Destructor which makes sure the data acquisition
     * stops on exit.
     **/
    ~GPIOPin();

    int getValue() {
	int val = gpiod_line_get_value(pinGPIO);
	if (val < 0) {
	    perror("Read line input failed.\n");
	}
	return val;
    }

    int setValue(int v) {
	int ret = gpiod_line_set_value(pinGPIO,v);
	if (ret < 0) {
	    perror("Write line output failed.\n");
	}
	return ret;
    }
    
    struct GPIOEventCallbackInterface {
	/**
	 * Called when a new sample is available.
	 * This needs to be implemented in a derived
	 * class by the client. Defined as abstract.
	 * \param e If falling or rising.
	 **/
	virtual void hasEvent(gpiod_line_event& e) = 0;
    };

    void registerCallback(GPIOEventCallbackInterface* ci) {
	adsCallbackInterfaces.push_back(ci);
    }

    /**
     * Starts listening on the GPIO pin.
     **/
    void start();

    /**
     * Stops listening to the pin.
     **/
    void stop();

private:
    void gpioEvent(gpiod_line_event& event);

    void worker();

    // gpiod stuff
    gpiod_chip *chipGPIO = nullptr;
    gpiod_line *pinGPIO = nullptr;

    // thread
    std::thread thr;

    // flag that it's running
    bool running = false;

    std::vector<GPIOEventCallbackInterface*> adsCallbackInterfaces;

    PinFunction pinFunction;

    std::string pinLabel;
};


#endif
