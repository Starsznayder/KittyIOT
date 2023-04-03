QT += core gui concurrent

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
DEFINES += WITH_GUI=1
INCLUDEPATH += /kitty/include
LIBS += -lboost_system -lboost_filesystem -lstdc++fs -lboost_thread -lboost_log -lpthread -lmodbus


SOURCES += \
    Config.cpp \
    DataBufer.cpp \
    Filter.cpp \
    MultiregOperator.cpp \
    Process.cpp \
    Triggers.cpp \
    FS.cpp \
    RegDef.cpp \
    ModbusReader.cpp \
    ChartsDef.cpp \
    main.cpp \
    FigureWindow/PolarPlot/PolarPlot.cpp \
    FigureWindow/PolarPlot/ChartView.cpp \
    FigureWindow/PolarPlot/Callout.cpp \
    FigureWindow/FigureWindow.cpp \
    FigureWindow/FigureGroup.cpp \
    FigureWindow/FigureData.cpp

HEADERS += \
    Config.h \
    DataBufer.h \
    Filter.h \
    MultiregOperator.h \
    Process.h \
    Triggers.h \
    FS.h \
    RegDef.h \
    ModbusReader.h \
    ChartsDef.h \
    FigureWindow/PolarPlot/PolarPlot.h \
    FigureWindow/PolarPlot/Callout.h \
    FigureWindow/PolarPlot/ui_polarwindow.h \
    FigureWindow/PolarPlot/ChartView.h \
    FigureWindow/FigureWindow.h \
    FigureWindow/FigureGroup.h \
    FigureWindow/TreeHeaderBase.h \
    FigureWindow/FigureData.h

FORMS += \
    FigureWindow/figurewindow.ui \
    FigureWindow/figuregroup.ui


linux-*{ # For Linux
    target.path = /kitty/IOT
    INSTALLS += target
}
