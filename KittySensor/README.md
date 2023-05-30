# KittySensor

The program can run a shell script with a given period, read standard output and parse numeric values. It can be used to read the measurement results from the sensor. These results will be sent via multicast.

Tested with
* DS18B20 temperature sensor


**Dependencies**

* Qt5 (Core)
* Boost 1.71
* digitemp
* KittyLogs (system management)

**Configuration**

Configuration file were created and placed in the ini directory.

`config.ini`


Sections Sensor`<X>` where X is the index of consecutive sensors, contains:
* name - the name of the sensor
* initScript - device initialization script
* readScript - reading measurement results, periodically run
* dataAccessInterval - readScript run interval
* deviceIndex - device logical index in the system (count from 0)

Section DataOutput contain multicast configuration (address and port).

**Bash Scripts**

Sample scripts for digitemp_DS18B20 (initialization and use) can be found in the sbin directory.

**Compilation**

Compilation can be done by the installation script or QtCreator

`cd KittySensor`
`./build.sh`

Executables, configuration files and shell scripts gonna land in /kitty/IOT.
The code was written for my private use only, in a specific, unspecified environment and I'm making it public now. It is possible that it will work in a different hardware configuration, but this has not been tested. On the other hand, it is possible that it will not work for you at all or even cause a hardware damage.
