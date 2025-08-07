#include "gpiopin.h"

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string>

GPIOPin::GPIOPin(int pinNo,
		 PinFunction func,
		 int chipNo) {
    
    pinFunction = func;
	
#ifdef DEBUG
    fprintf(stderr,"Init.\n");
#endif
    
    chipGPIO = gpiod_chip_open_by_number(chipNo);
    if (NULL == chipGPIO) {
#ifdef DEBUG
	fprintf(stderr,"GPIO chip could not be accessed.\n");
#endif
	throw "GPIO chip error.\n";
    }
    
    pinGPIO = gpiod_chip_get_line(chipGPIO,pinNo);
    if (NULL == pinGPIO) {
#ifdef DEBUG
	fprintf(stderr,"GPIO line could not be accessed.\n");
#endif
	throw "GPIO line error.\n";
    }

    int ret;
    switch (func) {
    case INPUT:
    case EVENT:
	pinLabel = "Input"+std::to_string(chipNo)+"_"+std::to_string(pinNo);
	ret = gpiod_line_request_input(pinGPIO, pinLabel.c_str());
	if (ret < 0) {
	    perror("Request line as input failed.\n");
	}
	break;
    case OUTPUT:
	pinLabel = "Output"+std::to_string(chipNo)+"_"+std::to_string(pinNo);
	ret = gpiod_line_request_output(pinGPIO, pinLabel.c_str(), 0);
	if (ret < 0) {
	    perror("Request line as input failed.\n");
	}
	break;
    }
}


GPIOPin::~GPIOPin() {
    stop();
    gpiod_line_release(pinGPIO);
    gpiod_chip_close(chipGPIO);
}


void GPIOPin::start() {
    if (EVENT != pinFunction) return;
    int ret = gpiod_line_request_both_edges_events(pinGPIO, pinLabel.c_str());
    if (ret < 0) {
#ifdef DEBUG
	fprintf(stderr,"Request event notification failed on pin %d and chip %d.\n",
		pinNo,chipNo);
#endif
	throw "Could not request event for IRQ.";
    }
    
    running = true;
    
    thr = std::thread(&GPIOPin::worker,this);
}

void GPIOPin::gpioEvent(gpiod_line_event& event) {
	for(auto &cb: adsCallbackInterfaces) {
	    cb->hasEvent(event);
	}
}


void GPIOPin::worker() {
    while (running) {
	const timespec ts = { ISR_TIMEOUT, 0 };
	// this blocks till an interrupt has happened!
	int r = gpiod_line_event_wait(pinGPIO, &ts);
	// check if it really has been an event
	if (1 == r) {
	    gpiod_line_event event;
	    gpiod_line_event_read(pinGPIO, &event);
	    gpioEvent(event);
	} else if (r < 0) {
#ifdef DEBUG
	    fprintf(stderr,"GPIO error while waiting for event.\n");
#endif
	}
    }
}


void GPIOPin::stop() {
    if (!running) return;
    running = false;
    thr.join();
}
