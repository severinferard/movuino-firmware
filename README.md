# Movuino firmware

Firmware for the Movuino wireless sensor board.

## Requirements

#### Linux

You'll need permission to access the serial device. Add yourself to the right group then relog.

##### Arch

`$ gpasswd -a $USER uucp`

##### Ubuntu

`$ usermod -a -G dialout $USER`

#### Windows

Install [CP210x USB to UART Bridge VCP Drivers](https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers) driver.

#### OS X

Install [CP210x USB to UART Bridge VCP Drivers](https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers) driver.

#### Firmware

1. [Install the ESP8266 Arduino Addon](https://learn.sparkfun.com/tutorials/esp8266-thing-hookup-guide/installing-the-esp8266-arduino-addon).

2. Select the `Adafruit HUZZAH ESP8266` board in Arduino IDE.

3. Copy the content of [Arduino/libraries](https://github.com/topela/movuino.js/tree/master/Arduino/libraries) into your Arduino libraries folder. See [Where to Install your Libraries](https://learn.adafruit.com/adafruit-all-about-arduino-libraries-install-use/how-to-install-a-library).

Once you selected the right port, you should be able to compile and upload the firmware.

## OSC namespace

The namespace is the same for serial and wifi connections.
OSC messages are transmitted in both directions by any possible mean, using SLIP encoded Serial and/or WiFi UDP (see examples).
OSC routes are defined in the file `globals.h`, and the routing mostly happens in the `Router::routeOSCMessage` method, implemented in `Router.cpp`.

#### input

* `/wifi/enable <onOff(bool)>`
* `/wifi/set <ssid(string)> <password(string)> <hostIP(string)> <inputUDPPort(int)> <outputUDPPort(int)>`
* `/wifi/get`
* `/range/set <accelRange(int)> <gyroRange(int)>`
* `/range/get`
* `/config/set <useSerial(bool)> <sendSingleFrame(bool)> <readMagPeriod(int)> <outputFramePeriod(int)> <buttonHoldTimeout(int)>`
* `/config/get`
* `/vibroPulse <onDuration(int)> <offDuration(int)> <times(int)>`
* `/vibroNow <onOff(bool)>`

Movuino responds to `/wifi/enable` and all the `/xxxx/set` messages by forwarding them back with their original values to the sender as an acknowledgement.

#### output

Acknowledgement messages apart, the output namespace is :

* `/wifi/state <wifiConnectionState(int)>`
* `/wifi/get <ssid(string)> <password(string)> <hostIP(string)> <inputUDPPort(int)> <outputUDPPort(int)>`
* `/range/get <accelRange(int)> <gyroRange(int)>`
* `/config/get <useSerial(bool)> <sendSingleFrame(bool)> <readMagPeriod(int)> <outputFramePeriod(int)> <buttonHoldTimeout(int)>`
* `/sensors <ax(float)> <ay(float)> <az(float)> <gx(float)> <gy(float)> <gz(float)> <mx(float)> <my(float)> <mz(float)>`
* `/button <buttonState(int)>`
* `/frame  <ax(float)> <ay(float)> <az(float)> <gx(float)> <gy(float)> <gz(float)> <mx(float)> <my(float)> <mz(float)> <buttonState(int)> <vibratorState(bool)>`

## credits

Based on the work of Adrien Husson and Sonny Piers.