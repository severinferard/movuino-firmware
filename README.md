# Movuino firmware

Firmware for the Movuino WiFi board.

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

## OSC namespaces

The Open Sound Control protocol is used for both serial and wifi communication. Messages are transmitted using either SLIP encoded serial or wifi UDP (see examples).
OSC routes are defined in the file `firmware/globals.h`, and the routing mostly happens in the `Router::routeOSCMessage` method, implemented in `firmware/Router.cpp`.

#### configuration namespace

The configuration is done via the serial connection only (an alternative is to use the Movuino in AP mode, see below), and has a specific OSC namespace. The Movuino processes every incoming message and always sends back a response or the message itself as an acknowledgement :

* `/hello` -> `/hello movuino <movuino_id> <wifi_state> <movuino_ip> <movuino_uuid> <firmware_version>`
* `/id/get` -> `/id/get <movuino_id>`
* `/id/set <movuino_id>` -> `/id/set <movuino_id>`
* `/wifi/get` -> `/wifi/get <network_ssid> [<optional_password>] <host_ip>`
* `/wifi/set <network_ssid> [<optional_password>] <host_ip>` -> `/wifi/set <network_ssid> [<optional_password>] <host_ip>`
* `/ports/get` -> `/ports/get <input_UDP_port> <output_UDP_port>`
* `/ports/set <input_UDP_port> <output_UDP_port>` -> `/ports/set <input_UDP_port> <output_UDP_port>`
* `/range/get` -> `/range/get <accel_range> <gyro_range>`
* `/range/set <accel_range> <gyro_range>` -> `/range/set <accel_range> <gyro_range>`
* `/frameperiod/get` -> `/frameperiod/get <frame_period>`
* `/frameperiod/set <frame_period>` -> `/frameperiod/set <frame_period>`
* `/wifi/enable <on_off>` -> `/wifi/enable <on_off>`
* `/serial/enable <on_off>` -> `/serial/enable <on_off>`
* `/magneto/enable <on_off>` -> `/magneto/enable <on_off>`

A last message is used to allow for monitoring of the wifi connection state via serial, which is fired automatically whenever the wifi state changes : `/wifi/state <wifi_state>`.

#### communication namespace

The communication namespace consists in 3 addresses.

##### input

* The `/vibro/now <on_off>` message enables or disables the vibrator.
* The `/vibro/pulse <on_duration> <off_duration> <nb_of_pulses>` message trigs a series of vibrations. If `<nb_of_pulses>` equals `-1`, The pulse goes on until a new message with another value is received.

##### output

* The `/movuino <movuino_id> <acc_x> <acc_y> <acc_z> <gyr_x> <gyr_y> <gyr_z> <mag_x> <mag_y> <mag_z> <button_state> <vibrator_state>` message is output every `<frame_period>` milliseconds.

#### notes

* All `/xxxx/set` messages, as well as `/wifi/enable`, `/serial/enable` and `/magneto/enable`, define new configuration parameter values that are automatically persisted.
* The `/wifi/enable` message starts (or stops) the wifi connection using the current wifi settings. The last stored value will be used to determine whether to start the wifi connection at boot time or not.
* The `/serial/enable` message never disables the serial connection. It only allows to use the communication namespace via serial (which can cause the OSC output rate to drop if the frame period parameter has a value of less than 10ms).
* The `/magneto/enable` message enables reading the magnetometer values. When it is disabled, the magnetometer values are all set to zero.
* `<host_ip>` has the form `a.b.c.d`, where `a`, `b`, `c` and `d` are integer values within the [0;255] range.
* `<accel_range>` can take the following values
    * `0` (+/- 2g)
    * `1` (+/- 4g)
    * `2` (+/- 8g)
    * `3` (+/- 16g)
* `<gyro_range>` can take the following values
    * `0` (+/- 250 deg/sec)
    * `1` (+/- 500 deg/sec)
    * `2` (+/- 1000 deg/sec)
    * `3` (+/- 2000 deg/sec)
* `<wifi_state>` can take the following values
    * `0` (disconnected)
    * `1` (connected)
    * `2` (connecting)
* `<button_state>` can take the following values
    * `0` (released)
    * `1` (pressed)
    * `2` (pressing for longer than 1 second)
* `<vibrator_state>` can take the following values
    * `0` (not vibrating)
    * `1` (vibrating)

## AP mode

The firmware also allows to boot in Access Point mode, and serve a configuration web page where all the settings can be modified and stored to the local configuration file. The served configuration page is stored in a static string variable in `firmware/configWebPage.h`, which is copy-pasted from `firmware/configWebPage.html`. The AP server code is defined in `firmware/AccesPoint.h` and `firmware/AccessPoint.cpp`.

<div style="text-align: center;">
<img src="https://raw.githubusercontent.com/josephlarralde/movuino-firmware/master/movuino-ap-interface.jpg">
</div>

To boot in AP mode, turn the movuino on while holding the button, and wait until the blue LED starts blinking quickly to release it.
The movuino will create a network named `movuino-xxxxxxxxxx`. Connect to this network, then visit the url `http://192.168.1.1` and you should see the configuration page appear, which will let you modify and store all the settings available via serial OSC.

## TODO's

* Add websocket support to OSC messages
* Add more vibration modes
* ...

## credits

Based on the work of Adrien Husson and Sonny Piers.
