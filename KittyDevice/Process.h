#ifndef PROCESS_H
#define PROCESS_H

#include <QObject>
#include <QSharedPointer>
#include <QString>

#include "MultiregOperator.h"
#include "Filter.h"
#include "Triggers.h"
#include "SunTrigger.h"

class Process : public QObject
{
    Q_OBJECT
public:
    Process(int _triggerIndex);
    Process(QSharedPointer<ConfigValue<SunTrigger::Trigger>> _sunTrigger) : sunTrigger(_sunTrigger), isAlreadyOn(false) {}
    void onRun();
    void onRunBySun();

signals:
    void command(QSharedPointer<kitty::network::object::ModbusMulticastCommand> values);

public slots:
    void onData(SensorsData values);
    void onData(kitty::network::object::Sun values);

private:
    int triggerIndex;
    bool isAlreadyOn;
    uint64_t turnOnTimestamp;
    double runtimeCounter;
    int day;
    uint64_t updateCounter;

    void turnOn(const Triggers::Trigger& trigger);
    void turnOff(const Triggers::Trigger& trigger);
    void turnOnBySun(const int64_t dT);
    void turnOffBySun(const int64_t dT);
    bool timerCondition(uint64_t timestamp, const  Triggers::Trigger& trigger);

    QSharedPointer<ConfigValue<SunTrigger::Trigger>> sunTrigger;
};

#endif // PROCESS_H
