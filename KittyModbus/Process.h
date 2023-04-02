#ifndef PROCESS_H
#define PROCESS_H

#include <QObject>
#include <QSharedPointer>

#include "MultiregOperator.h"
#include "Filter.h"
#include "Triggers.h"

class Process : public QObject
{
    Q_OBJECT
public:
    Process(int _triggerIndex);

public slots:
    void onData(ModbusData values);

private:
    int triggerIndex;
    bool isAlreadyOn;
    uint64_t turnOnTimestamp;
    double runtimeCounter;
    int day;
    uint64_t updateCounter;
    bool timerCondition(uint64_t timestamp, const  Triggers::Trigger& trigger);
};

#endif // PROCESS_H
