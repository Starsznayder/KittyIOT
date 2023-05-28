#ifndef SENSORREADER_H
#define SENSORREADER_H

#include <QObject>
#include <QSharedPointer>
#include <QVector>
#include <QFuture>
#include <atomic>
#include <mutex>
#include <queue>
#include "MuticastMessageParser.h"

class SensorReader : public QObject
{
Q_OBJECT
public:
    SensorReader();
    ~SensorReader()
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

private:
    void worker();
    QFuture<void> readerThreadObject;
    std::atomic<bool> finish;
};

#endif // SENSORREADER_H
