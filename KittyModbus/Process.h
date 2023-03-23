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
    Process(Triggers::Trigger _triger);

public slots:
    void onData(ModbusData values);

private:
    Triggers::Trigger trigger;
    bool isAlreadyOn;
    uint64_t turnOnTimestamp;
    double runtimeCounter;
    int day;
    uint64_t updateCounter;
    bool timerCondition(uint64_t timestamp);
};

#endif // PROCESS_H
