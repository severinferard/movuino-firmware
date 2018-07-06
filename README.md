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
* `/wifi/set <ssid(string)> [ <password(string)> ] <hostIP(string)>`
* `/wifi/get`
* `/ports/set  <inputUDPPort(int)> <outputUDPPort(int)>`
* `/ports/get`
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
* `/wifi/get <ssid(string)> <password(string)> <hostIP(string)>`
* `/ports/get <inputUDPPort(int)> <outputUDPPort(int)>`
* `/range/get <accelRange(int)> <gyroRange(int)>`
* `/config/get <useSerial(bool)> <sendSingleFrame(bool)> <readMagPeriod(int)> <outputFramePeriod(int)> <buttonHoldTimeout(int)>`
* `/sensors <ax(float)> <ay(float)> <az(float)> <gx(float)> <gy(float)> <gz(float)> <mx(float)> <my(float)> <mz(float)>`
* `/button <buttonState(int)>`
* `/frame  <ax(float)> <ay(float)> <az(float)> <gx(float)> <gy(float)> <gz(float)> <mx(float)> <my(float)> <mz(float)> <buttonState(int)> <vibratorState(bool)>`

All the `/xxxx/get` messages are responses to the corresponding input messages.

#### notes

* Except `/vibroPulse` and `/vibroNow`, all the input messages are directly taken into account and stored to a configuration file which will be loaded on reboot.
* Parameters of type `bool` use int values (`0` for `false` and `1` for `true`).
* `hostIP` has the form `a.b.c.d`, where `a`, `b`, `c` and `d` are integer values within the [0;255] range.
* `accelRange` can take the following values
    * `0` (+/- 2g)
    * `1` (+/- 4g)
    * `2` (+/- 8g)
    * `3` (+/- 16g)
* `gyroRange` can take the following values
    * `0` (+/- 250 deg/s)
    * `1` (+/- 500 deg/s)
    * `2` (+/- 1000 deg/s)
    * `3` (+/- 2000 deg/s)
* All the parameters named `xxxPeriod` and `xxxDuration` are expressed in milliseconds.
* If the specified `times` parameter in the `/vibroPulse` message equals `-1`, the pulse will go on forever until a new message with another value is received
* `/wifi/state` is the only message not sent via WiFi, as it gives the WiFi connection state in real-time (0 is disconnected, 1 is connected, and 2 is connecting).
* When the `sendSingleFrame` option is enabled, sensor values and the button state are sent altogether via the `/frame` message, at 1000 * `outputFramePeriod` Hz.
* If `sendSingleFrame` is disabled, sensor values are sent via the `/sensors` message at 1000 * `outputFramePeriod` Hz, and the button state is sent via the `/button` message on value change only.
* For the time being webSocket transmission of OSC messages is not implemented, so if there is a lot of traffic on the network, it is advised to enable `sendSingleFrame` to avoid losing button state messages, and to configure the board through serial connection for the same reason.

## AP mode

The firmware also allows to boot in Access Point mode, providing access to a configuration page where all the settings can be modified and stored to the local configuration file.
![Movuino AP interface](https://raw.githubusercontent.com/josephlarralde/movuino-firmware/master/)
To boot in AP mode, turn the movuino on while holding the button, and wait until the blue LED starts blinking quickly to release it.
The movuino will create a network named `movuino-xxxxxxxxxx`. Connect to this network, then visit the url `http://192.168.1.1` and you should see the configuration page appear, which will let you modify and store all the settings available via OSC.

## TODO's

* Add websocket support to OSC messages
* Simplify API
* Add more vibration modes
* ...

## credits

Based on the work of Adrien Husson and Sonny Piers.
