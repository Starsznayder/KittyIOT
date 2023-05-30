# KittyIOT

A set of helpful Internet Of Things/Smart Home services. They communicate via multicast so they can be run on one or more devices at once in different configurations.

**KittyWeather**

It uses the OpenWeather JSON API to download the current weather forecast, save it to disk and broadcast over the network.

**KittySensor**

It allows reading from the sensor (e.g. temperature or humidity), and broadcasts colected informations in the multicast.

**KittyModbus**

Information obtained from devices is sent via multicast, i.e. the same with which they can be controlled.

**KittDevice**

Receives information from sensors via multicast and controls devices (e.g. air conditioner, cryptocurrency miner etc.)


**Dependencies**
* boost 1.71
* Qt5 (Core)
* OpenWeather (JSON API)
* libmodbus (MODBUS)
* socat (RS485/Ethernet converter)
* egctl (EG-PMS2)
* digitemp (temperature sensor)
* KittyLogs (system management)

`./depinstall.sh`

**Compilation**
Compilation can be done by the installation script or QtCreator


`cd <module name>`
`./build.sh`

Executables, configuration files and shell scripts gonna land in /kitty/IOT.


The code was written for my private use only, in a specific, unspecified environment and I'm making it public now. It is possible that it will work in a different hardware configuration, but this has not been tested. On the other hand, it is possible that it will not work for you at all or even cause a hardware damage.
