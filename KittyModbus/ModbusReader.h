#ifndef MODBUSREADER_H
#define MODBUSREADER_H

#include <QObject>
#include <QSharedPointer>
#include <QVector>
#include <QFuture>
#include <atomic>
#include <mutex>
#include <queue>
#include "MuticastMessageParser.h"

class ModbusReader : public QObject
{
Q_OBJECT
public:

    struct Command
    {
        unsigned idx;
        unsigned addr;
        uint16_t value;
    };

    ModbusReader();
    ~ModbusReader()
    {
        finish.store(true);
        readerThreadObject.waitForFinished();
    }

signals:
    void freshData(SensorsData values);
    void sensorsMulticastMSG(QSharedPointer<kitty::network::object::SensorsMulticastMSG>);

public slots:
    void onDisconnect();
    void onConnect();
    void onStop(unsigned, unsigned);
    void onStart(unsigned, unsigned);


private:
    void worker();
    QFuture<void> readerThreadObject;
    std::atomic<bool> finish;
    std::mutex proctor;
    std::queue<Command> commands;
};

#endif // MODBUSREADER_H
