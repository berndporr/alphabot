#ifndef __LOGIWHEEL
#define __LOGIWHEEL

#include <thread>
#include <functional>
#include <string>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/joystick.h>
#include <sys/select.h>

class LogiWheel {

public:

    using CallbackFunction = std::function<void(float)>;
    
    void registerSteeringCallback(CallbackFunction cb) {
	steeringCallback = cb;
    }

    void registerThrottleCallback(CallbackFunction cb) {
	throttleCallback = cb;
    }

    void registerBrakeCallback(CallbackFunction cb) {
	brakeCallback = cb;
    }

    void start(std::string device = "/dev/input/by-id/usb-Logitech_Logitech_Racing_Wheel-event-joystick") {
	if (running) return;

        fd = open(device.c_str(), O_RDONLY);
	
	if (fd == -1) {
	    perror("Could not open joystick");
	    return;
	}
	eventThread = std::thread(&LogiWheel::run, this);
    }
    
    void stop() {
	running = false;
	eventThread.join();
    }

private:

    void run() {
	running = true;
	while (running) 
	{
	    fd_set s_rd;
	    FD_ZERO(&s_rd);
	    FD_SET(fd, &s_rd);
	    int r = select(fd+1, &s_rd, NULL, NULL, &timeout);
	    if (r > 0) {
		r = read(fd, &event, sizeof(event));
		if (r < 0) {
		    fprintf(stderr,"Logi Wheel err = %d\n",r);
		    running = false;
		    close(fd);
		    return;
		}
		if (event.type == EV_ABS && event.code < ABS_TOOL_WIDTH) {
		    int v = event.value;
		    switch (event.code) {
		    case 0:
			if (steeringCallback) steeringCallback((v - 512)/512.0);
			break;
		    case 1:
			if (throttleCallback) throttleCallback(1-v/256.0);
			break;
		    case 2:
			if (brakeCallback) brakeCallback(1-v/256.0);
			break;
		    }
		}
	    } else if (r < 0) running = false;
	}
	running = false;
	close(fd);
    }

    bool running = false;
    int fd = -1;
    input_event event;
    std::thread eventThread;
    timeval timeout = {1,1};

    CallbackFunction steeringCallback;
    CallbackFunction throttleCallback;
    CallbackFunction brakeCallback;
};

#endif
