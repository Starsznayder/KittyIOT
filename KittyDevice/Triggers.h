#ifndef TRIGGERS_H
#define TRIGGERS_H

#include "Config.h"
#include <QString>
#include <QVector>
#include <QSharedPointer>
#include "Filter.h"
#include "MultiregOperator.h"
#include <QFuture>
#include <QtConcurrent>
#include <atomic>

class Triggers
{
public:

    Triggers(const Triggers&) = delete;
    Triggers& operator=(const Triggers&) = delete;

    static QVector<int> str2regNumers(std::string value);

    static Triggers& instance()
    {
        static Triggers c;
        return c;
    }

    struct Trigger
    {
        enum class Mode
        {
            SCRIPT,
            MODBUS
        };

        enum class ThresholdType
        {
            LO,
            GT
        };

        QString name;
        Mode mode;
        unsigned onOffRegisterAddr;
        unsigned modbusDevID;
        int srcDeviceIndex;
        QVector<int> registers;
        QSharedPointer<operators::MultiregOperator> multiregOperator;
        QSharedPointer<filters::Filter> filter;
        int order;
        float turnOnThreshold;
        ThresholdType turnOnThresholdType;
        QString turnOnRunPath;
        float turnOffThreshold;
        ThresholdType turnOffThresholdType;
        QString turnOffRunPath;
        int minRunTimePerDay;
        int runTimeFulfillmentTimeBorder;
        bool manualMode;
        bool oN;

        void update(QString _name,
                    Mode _mode,
                    unsigned _onOffRegisterAddr,
                    unsigned _modbusDevID,
                    int _srcDeviceIndex,
                    QVector<int> _registers,
                    filters::FilterType filterType,
                    operators::OperatorType operatorType,
                    int _order,
                    float _turnOnThreshold,
                    ThresholdType _turnOnThresholdType,
                    QString _turnOnRunPath,
                    float _turnOffThreshold,
                    ThresholdType _turnOffThresholdType,
                    QString _turnOffRunPath,
                    int _minRunTimePerDay,
                    int _runTimeFulfillmentTimeBorder,
                    bool manualMode,
                    bool oN);
    };

    QVector<QSharedPointer<ConfigValue<Trigger>>> triggers;

private:
    QFuture<void> readerThreadObject;
    std::atomic<bool> finish;

    void loadFile(const std::string& configFilepath);

    Triggers()
    {
        loadFile("/kitty/IOT/KittyDevice/ini/triggers.ini");
        finish.store(false);
        readerThreadObject = QtConcurrent::run(this, &Triggers::updateThread);
    };

    void updateThread()
    {
        while (!finish.load())
        {
            QThread::sleep(5);
            loadFile("/kitty/IOT/KittyDevice/ini/triggers.ini");
        }
    }
    static Trigger::ThresholdType str2thresholdType(const std::string& value);
    static Trigger::Mode str2mode(const std::string& value);
};

#endif // TRIGGERS_H
