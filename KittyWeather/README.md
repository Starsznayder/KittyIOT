# KittyWeather

It uses the OpenWeather JSON API to download the current weather forecast, saves it on HDD and broadcasts over the network.


**Dependencies**

* boost 1.71
* Qt5 (Core)
* OpenWeather (JSON API) 

**Configuration**

Configuration file were created and placed in the ini directory.

`weatherConfig.ini`

Section DataOutput contain multicast configuration (address and port).

**Compilation**

Compilation can be done by the installation script or QtCreator

`cd KittyWeather`
`./build.sh`

Executables, configuration files and shell scripts gonna land in /kitty/IOT.
