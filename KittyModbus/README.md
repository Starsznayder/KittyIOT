# KittyModbus
A simple controller for electrical devices using an electricity import/export meter, equipped with some diagnostic tools. The purpose of its existence is to start or stop electrical devices depending on the time of day and the availability of electricity (for example from a PV installation). 

Tested on a network composed of:
* EASTRON SDM630M Modbus-V2 electric counter
* USR-DR302 RS485/Ethernet converter
* F&F MAX-MR-RO-1 Modbus switch
* Single board PC running Ubuntu 20.04


**Dependencies**

* libmodbus (communiation with the electric counter)
* Qt5 (Core)
* Boost 1.71
* socat (RS485/Ethernet converter)
* KittyLogs (system management)

**Configuration**

Configuration files were created and placed in the ini directory.

`config.ini`

This file contains the modbus device, multicast and logs configuration:

Sections Modbus`<X>` constains configuration of the interface of each device, where X is the logical identyficator of the device in this system. Devices can run on one or more buses simultaneously, but they are always polled in the order defined in this file.

Section DataOutput contain multicast configuration (address and port).

`regDef<X>.ini`

Thouse files specifies the registers to read in each device, where X is the logical device ID. (by default they describes the most of the avaliable EASTRON SDM630M Modbus-V2 and F&F MAX-MR-RO-1 options)

* name - the name of the register content
* unit - unit (eg. V or kWh)
* regAdr - address
* size - size in bytes
* type - float or int

**Bash Scripts**

Sample scripts for socat configuration (USR-DR302) can be found in the sbin directory.

**Compilation**

Compilation can be done by the installation script or QtCreator

`cd KittyModbus`
`./build.sh`

Executables, configuration files and shell scripts gonna land in /kitty/IOT.
The code was written for my private use only, in a specific, unspecified environment and I'm making it public now. It is possible that it will work in a different hardware configuration, but this has not been tested. On the other hand, it is possible that it will not work for you at all or even cause a hardware damage.
