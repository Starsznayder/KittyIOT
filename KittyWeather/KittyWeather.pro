QT += core network concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 console

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
DEFINES += BOOST_LOG_DYN_LINK

INCLUDEPATH += /kitty/include
INCLUDEPATH += ../network
INCLUDEPATH += ../commons
LIBS += -lboost_system -lboost_filesystem -lstdc++fs -lboost_thread -lboost_log -lpthread

SOURCES += \
    ../network/WeatherMulticastMSG.cpp \
    Config.cpp \
    ../commons/FS.cpp \
    WeatherJSONParser.cpp \
    main.cpp \
    Downloader.cpp \
    ../network/SimpleUdpServer.cpp \
    ../network/SensorsMulticastMSG.cpp \
    ../network/ModbusMulticastMSG.cpp \
    ../network/MuticastMessageParser.cpp

HEADERS += \
    ../network/WeatherMulticastMSG.h \
    Config.h \
    Downloader.h \
    ../commons/FS.h \
    WeatherJSONParser.h \
    ../network/SimpleUdpServer.h \
    ../network/SensorsMulticastMSG.h \
    ../network/ModbusMulticastMSG.h \
    ../network/MuticastMessageParser.h


linux-*{ # For Linux
    target.path = /kitty/IOT/Weather
    INSTALLS += target
}
