# C++ Alphabot API for the Raspberry PI

![alt tag](robot.jpg)

C++ class to control the basic functionality of the Alphabot
(https://www.waveshare.com/wiki/AlphaBot) with a Raspberry PI:

 - PWM motor control
 - Distance sensor readings (digital and analogue)
 - Battery voltage
 - Wheel speed encoders (rotating flags)
 - IR sensor readings
 - 10Hz sampling rate with callback

## Prerequisites

### pigpio
The Raspberry PI hardware is accessed via the
`pigpio` library. Install its package:
```
apt-get install libpigpio-dev
```

### Timer library
Install the timer library from:
https://github.com/berndporr/cppTimer

### ncurses
The demo programs below display the sensor
readings with the ncurses library. Install
it with
```
apt-get install libncurses-dev
```

## Building

The built system is `cmake`. Just type:
```
cmake .
make
```

## Usage

The online documentation is here: https://berndporr.github.io/alphabot/

### Start/stop

Start the communication with the robot:
```
start()
```

Stop the communication:
```
stop()
```

### Motor speed

Setting the speeds of the left/right wheels:
```
setLeftWheelSpeed(float speed);
setRightWheelSpeed(float speed);
```
where speed is between -1 and +1.

### Get Collision sensor readings

The digital functions are boolean values which are true if the
distance sensor threshold (set with the potentiometer) has been reached:
```
bool getCollisionLeft();
bool getCollisionRight();
```

The analogue readings from the sensors from 0..1 can be read with:
```
float getLeftDistance()
float getRightDistance()
```

### Battery voltage

The function `getBatteryLevel()` provides the voltage of the battery
in volt.

### Speed encoders

These are terrible encoders and really only reliable to detect if the
wheel is spinning at all or not. Updates every 1/2 sec.
```
bool getLeftWheelSpinning();
bool getRightWheelSpinning();
```

### Infrared channels / general purpose ADC channels 0-4

```
float (&getIR())[nIR]
```
returns a reference to the whole array of IR sensor readings
normalised between 0..1.

### Callback

Whenever a new set of analogue readings is available the callback `step`
in `StepCallback` is called. It contains the reference to the Alphabot
class itself so that one can read the different ADC values.

## Demo programs

![alt tag](testIO.png)

`testIO` is a simple test program which displays the different
senor readings and you can test the motors. It also shows
how the callback is used to display the sensor readings.

`testMotor` ramps up the motor speed and back again.

`obstacleAvoid.cpp` implements a Braitenberg vehicle
and it has an emergency stop if the digital readings from
the collision sensors are triggered. This is an
instructional example and needs to be tweaked. The
fun begins here! :)
