#ifndef PROCESS_H
#define PROCESS_H

#include <QObject>
#include <QSharedPointer>
#include <QString>
#include "MuticastMessageParser.h"

class Process : public QObject
{
    Q_OBJECT
public:
    Process();

signals:
    void start(unsigned, unsigned);
    void stop(unsigned, unsigned);

public slots:

    void onData(QSharedPointer<kitty::network::object::ModbusMulticastCommand> values);

private:
    QVector<kitty::network::object::ModbusMulticastCommand> stateMemory;
};

#endif // PROCESS_H
