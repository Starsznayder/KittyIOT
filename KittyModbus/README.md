# KittyIOT
A simple controller for electrical devices using an electricity import/export meter, equipped with some diagnostic tools. The purpose of its existence is to start or stop electrical devices depending on the time of day and the availability of electricity (for example from a PV installation). 

Tested on a network composed of:
* EASTRON SDM630M Modbus-V2 electric counter
* USR-DR302 RS485/Ethernet converter
* EnerGenie EG-PMS2-LAN ethernet controlled power strip
* Single board PC running Ubuntu 20.04


**Dependencies**

* libmodbus (communiation with the electric counter)
* Qt5 (GUI)
* Qt5Charts (charts)
* Boost 1.71
* socat (RS485/Ethernet converter)
* egctl (EG-PMS2)

**Configuration**

Four configuration files were created and placed in the ini directory.

`config.ini`

This file contains the modbus configuration, the settings should be identical to those on the electricity meter. In addition, the following fields are available:

* ifaceName - path to the RS485 (by default, this is the path to the virtual interface created by the attached scripts),
* dataAccessInterval - energy meter register reading interval (default 5s),
* dataHistoryBufferSize - chart buffer size (two days by default).

`regDef.ini`

This file specifies the registers to read (by default it contains the most of the avaliable EASTRON SDM630M Modbus-V2 input registers)

* name - the name of the register content
* unit - unit (eg. V or kWh)
* regAdr - address


`charts.ini`

This file allows to select content to display in the charts.
* name - chart name
* registers - selects sections from regDef.ini (max 6 comma separated values)

By default, the graphs of voltage, frequency, import/export and power for individual phases are defined.

`triggers.ini`

This file specifies the target actions and how they are triggered.

* registers - registers to observe (multiple comma separated section numbers from regDef.ini)
* operator - how to combine readings from multiple registers (enum: MIN, MAX, MEAN, SUM)
* filter - time domain filter (enum: MEDIAN, MEAN)
* order - filter order
* turnOnThreshold - threshold value for the enabling action
* turnOnThresholdType - greater or lower (enum: GT, LO)
* turnOnRunPath - path to the script to run when the turnOnThreshold is exceeded
* turnOffThreshold - threshold value for the disabling action
* turnOffThresholdType - greater or lower (enum: GT, LO)
* turnOffRunPath - path to the script to run when the turnOffThreshold is exceeded
* minRunTimePerDay - If the sum of the time between the launch of the enabling script and the shutdown script to the time specified in the field runTimeFulfillmentTimeBorder is less than the value in the field minRunTimePerDay, the missing time difference will be made up after the time specified in the field runTimeFulfillmentTimeBorder (default 2h)
runTimeFulfillmentTimeBorder - (default 19:00)

**Bash Scripts**

Sample device startup scripts can be found in the sbin directory

**Compilation**
Compilation can be done by the installation script or QtCreator

`cd KittyModbus`
`./build.sh`

Executables, configuration files and shell scripts gonna land in /kitty/IOT.


The code has been prepared for use with the previously mentioned devices, in a specific, unspecified environment. It is possible that it will work in a different hardware configuration, but this has not been tested. On the other hand, it is possible that it will not work for you at all or even caue a hardware damage.
