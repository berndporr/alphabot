# alphabot
C++ class to access the basic functionality of the alphabot:
https://www.waveshare.com/wiki/AlphaBot

 - PWM motor control
 - Distance sensor readings (digital and analogue)
 - Battery voltage
 - Wheel speed encoders

## Prerequisites

The Raspberry PI hardware is accessed via the
`pigpiod` which is the version of the `pigpio` library
which runs as a demon. Install the following packages:
```
apt-get install libpigpio-dev
apt-get install libpigpiod-if-dev
apt-get install libpigpiod-if2-1
```
and enable and start the `pigpiod` with:
```
systemctl enable pigpiod
systemctl start pigpiod 
```

## Usage

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

The alalogue readings from the sensors from 0..1 can be read with:
```
float getLeftDistance()
float getRightDistance()
```

### Battery voltage

The function `getBatteryLevel()` provides the voltage of the battery
in volt.

### Speed encoders

These are horrible mechanical wheels and their signals are unreliable:
```
int getActualLeftWheelSpeed()
int getActualRightWheelSpeed()
```

### Callback

Whenever a new set of analogue readings is available the callback `step`
in `StepCallback` is called. It contains the reference to the Alphabot
class itself so that one can read the different ADC values.

## Demo program

`testIO` is a simple test program which displays the different
senor readings and you can test the motors.

ESC terminates the program and closes the connection.
