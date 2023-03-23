#ifndef MODBUSREADER_H
#define MODBUSREADER_H

#include <QObject>
#include <QSharedPointer>
#include <QVector>
#include <QFuture>
#include <atomic>

struct ModbusData
{
    uint64_t timestamp;
    QSharedPointer<QVector<float>> values;
};

class ModbusReader : public QObject
{
Q_OBJECT
public:
    ModbusReader();

signals:
    void freshData(ModbusData values);

public slots:
    void onDisconnect();
    void onConnect();

private:
    void worker();
    QFuture<void> readerThreadObject;
    std::atomic<bool> finish;

};

#endif // MODBUSREADER_H
