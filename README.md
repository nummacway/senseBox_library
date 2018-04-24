# SenseBox Library

Use Library for the following sensors:

- HDC100X temperature and humidity sensor
- VEML6070 ultraviolet light sensor
- CJMCU-3216/AP3216 ambient light sensor, behaving like TSL45315
- BMP280 temperature and air pressure sensor
- HC SR04 ultrasonic sensor
- DS1307 real-time clock, behaving like RV8523
- RGB LED

# Usage

Include library with `#include <SenseBox.h>`. Before the setup routine define the classes and classnames:

- `HDC100X HDC1(0,0);` define the I2C-Adress within the Brackets
- `Ultrasonic Ultrasonic(3,4);` set RX and TX Pins
- `TSL45315 luxsensor = TSL45315();`
- `VEML6070 uvsensor;`
- `RV8523 rtc;`
- `OpenSenseMap osem();` optional args: API domain & local IP

In the `setup()`-Routine initialise the sensor with `classname.begin()`. For example `HDC1.begin()`.
Now you can use `classname.getValue`. For example `HDC1.getTemp()`. See also the example folder.

# Credits
Uses RTClib from Adafruit
