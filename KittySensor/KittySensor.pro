QT += core gui concurrent network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets charts

CONFIG += c++14 console

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += BOOST_LOG_DYN_LINK

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#Compile With Gui, set 0 for network only version
#DEFINES += WITH_GUI=0
INCLUDEPATH += /kitty/include
INCLUDEPATH += ../network
INCLUDEPATH += ../commons
LIBS += -lboost_system -lboost_filesystem -lstdc++fs -lboost_thread -lboost_log -lpthread

SOURCES += \
    Config.cpp \
    ../commons/FS.cpp \
    SensorReader.cpp \
    KittySensor.cpp \
    ../network/SimpleUdpServer.cpp \
    ../network/ModbusMulticastMSG.cpp \
    ../network/SensorsMulticastMSG.cpp \
    ../network/WeatherMulticastMSG.cpp \
    ../network/MuticastMessageParser.cpp

HEADERS += \
    Config.h \
    ../commons/FS.h \
    SensorReader.h \
    ../network/SimpleUdpServer.h \
    ../network/ModbusMulticastMSG.h \
    ../network/SensorsMulticastMSG.h \
    ../network/MuticastMessageParser.h \
    ../network/WeatherMulticastMSG.h \
    ../commons/ConfigValue.h

linux-*{ # For Linux
    target.path = /kitty/IOT/KittySensor
    INSTALLS += target
}
