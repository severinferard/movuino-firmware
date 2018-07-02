# Movuino firmware

Firmware for the Movuino wireless sensor board, based on Adrien Husson and Sonny Piers' work.

### OSC namespace

The namespace is the same for serial and wifi connections. OSC messages are transmitted in both directions by any possible mean (see examples).

The OSC routes are defined in the file `globals.h`, and the routing mostly happens in the `Router::routeOSCMessage` method, implemented in `Router.cpp`.

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

Movuino responds to `/wifi/enable` and the `/xxxx/set` messages by forwarding them back with their original values to the sender as an acknowledgement.

#### output

