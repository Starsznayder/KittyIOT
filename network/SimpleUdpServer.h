#ifndef SIMPLEUDPSERVER_H
#define SIMPLEUDPSERVER_H

#include <QUdpSocket>
#include <QDataStream>
#include "ModbusMulticastMSG.h"
#include "SensorsMulticastMSG.h"
#include "WeatherMulticastMSG.h"

namespace kitty {
namespace network {
class SimpleUdpServer : public QObject
{
    Q_OBJECT
public:
    explicit SimpleUdpServer(QString addr, quint16 port, QString ifceAddr, QObject* parent = 0);
    ~SimpleUdpServer();

signals:
    void datagramReceived(const QByteArray& datagram);
    void multicastError();
    void recvModbusMulticastCommand(QSharedPointer<kitty::network::object::ModbusMulticastCommand>);
    void recvSensorsMulticastMSG(QSharedPointer<kitty::network::object::SensorsMulticastMSG>);
    void recvWeatherMulticastMSG(QSharedPointer<kitty::network::object::WeatherData>);

public slots:
    void readPendingDatagrams();
    void readAndDispathPendingDatagrams();
    void onStopListening();
    void onStartListening();
    void onStartListeningBoardcast();
    void onReload(QString addr, quint16 port, QString ifceAddr);
    void onModbusMulticastCommand(QSharedPointer<kitty::network::object::ModbusMulticastCommand>);
    void onSensorsMulticastMSG(QSharedPointer<kitty::network::object::SensorsMulticastMSG>);
    void onWeatherMulticastMSG(QSharedPointer<kitty::network::object::WeatherData>);
private:
    QUdpSocket* pUdpSocket_;
    QString addr_;
    quint16 port_;
    QString ifceAddr_;
    bool isConnected_;
    bool finish_;

    template<typename T>
    void sendMsg(T msg)
    {
        QByteArray buffer;
        QDataStream out(&buffer, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_0);
        out << *msg;
        QHostAddress groupAddress = QHostAddress(addr_);
        if (pUdpSocket_->state() != QAbstractSocket::BoundState)
        {
            pUdpSocket_->bind(QHostAddress(ifceAddr_), port_, QUdpSocket::ShareAddress);
        }
        pUdpSocket_->writeDatagram(buffer, groupAddress, port_);
    }

};

}
}

#endif // SIMPLEUDPSERVER_H
