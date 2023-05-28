#ifndef SUNTRIGGER_H
#define SUNTRIGGER_H

#include "Config.h"
#include <QString>
#include <QVector>
#include <QSharedPointer>
#include "Filter.h"
#include "MultiregOperator.h"
#include <QFuture>
#include <QtConcurrent>
#include <atomic>

class SunTrigger
{
public:

    SunTrigger(const SunTrigger&) = delete;
    SunTrigger& operator=(const SunTrigger&) = delete;

    static SunTrigger& instance()
    {
        static SunTrigger c;
        return c;
    }

    struct Trigger
    {
        enum class ThresholdType
        {
            RISE,
            FALL,
            NIGHT,
            DAY
        };

        enum class Mode
        {
            SCRIPT,
            MODBUS
        };

        QString name;
        Mode mode;
        unsigned onOffRegisterAddr;
        unsigned modbusDevID;
        QString actionPath;
        ThresholdType thresholdType;
        int offset;
        int delay;
        bool manualMode;
        bool oN;

        void update(QString _name,
                    Mode mode,
                    unsigned onOffRegisterAddr,
                    unsigned modbusDevID,
                    QString actionPath,
                    ThresholdType thresholdType,
                    int offset,
                    int delay,
                    bool manualMode,
                    bool oN);
    };

    QVector<QSharedPointer<ConfigValue<Trigger>>> triggers;

private:
    QFuture<void> readerThreadObject;
    std::atomic<bool> finish;

    void loadFile(const std::string& configFilepath);

    SunTrigger()
    {
        loadFile("/kitty/IOT/KittyDevice/ini/sunTriggers.ini");
        finish.store(false);
        readerThreadObject = QtConcurrent::run(this, &SunTrigger::updateThread);
    };

    void updateThread()
    {
        while (!finish.load())
        {
            QThread::sleep(5);
            loadFile("/kitty/IOT/KittyDevice/ini/sunTriggers.ini");
        }
    }

    static Trigger::ThresholdType str2thresholdType(const std::string& value);
    static Trigger::Mode str2mode(const std::string& value);
};

#endif // SUNTRIGGER_H
