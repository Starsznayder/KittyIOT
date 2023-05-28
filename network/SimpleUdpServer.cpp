#include "SimpleUdpServer.h"
#include <QNetworkInterface>
#include <thread>
#include <kittyLogs/log.h>

namespace kitty{
namespace network {
SimpleUdpServer::SimpleUdpServer(QString addr,
                                 quint16 port,
                                 QString ifceAddr,
                                 QObject* parent) :
    QObject(parent),
    addr_(addr),
    port_(port),
    ifceAddr_(ifceAddr),
    isConnected_(false),
    finish_(false)
{
    pUdpSocket_ = new QUdpSocket(this);

    qRegisterMetaType<kitty::network::object::ModbusMulticastCommand>("kitty::network::object::ModbusMulticastCommand");
    qRegisterMetaType<QSharedPointer<kitty::network::object::ModbusMulticastCommand>>("QSharedPointer<kitty::network::object::ModbusMulticastCommand>");
    qRegisterMetaType<kitty::network::object::SensorsMulticastMSG>("kitty::network::object::SensorsMulticastMSG");
    qRegisterMetaType<QSharedPointer<kitty::network::object::SensorsMulticastMSG>>("QSharedPointer<kitty::network::object::SensorsMulticastMSG>");
    qRegisterMetaType<kitty::network::object::WeatherData>("kitty::network::object::WeatherData");
    qRegisterMetaType<QSharedPointer<kitty::network::object::WeatherData>>("QSharedPointer<kitty::network::object::WeatherData>");

}

void SimpleUdpServer::onReload(QString addr, quint16 port, QString ifceAddr)
{
    addr_ = addr;
    port_ = port;
    ifceAddr_ = ifceAddr;
}

void SimpleUdpServer::readPendingDatagrams()
{
    while (pUdpSocket_->hasPendingDatagrams())
    {
        QByteArray datagram;

        datagram.resize(pUdpSocket_->pendingDatagramSize());

        QHostAddress sender;
        quint16 senderPort;
        pUdpSocket_->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

        emit datagramReceived(datagram);
    }
}

void SimpleUdpServer::readAndDispathPendingDatagrams()
{
    while (pUdpSocket_->hasPendingDatagrams())
    {
        QByteArray datagram;

        datagram.resize(pUdpSocket_->pendingDatagramSize());

        QHostAddress sender;
        quint16 senderPort;
        pUdpSocket_->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

        QDataStream in(datagram);
        in.setVersion(QDataStream::Qt_5_0);

        quint64 magic;
        in >> magic;
        if (kitty::network::object::ModbusMulticastCommand::isMagic(magic))
        {
            QSharedPointer<kitty::network::object::ModbusMulticastCommand> dataPtr =
                    QSharedPointer<kitty::network::object::ModbusMulticastCommand>::create();
            in >> *dataPtr;
            emit recvModbusMulticastCommand(dataPtr);
        }
        else if (kitty::network::object::SensorsMulticastMSG::isMagic(magic))
        {
            QSharedPointer<kitty::network::object::SensorsMulticastMSG> dataPtr =
                    QSharedPointer<kitty::network::object::SensorsMulticastMSG>::create();
            in >> *dataPtr;
            emit recvSensorsMulticastMSG(dataPtr);
        }
        else if (kitty::network::object::WeatherData::isMagic(magic))
        {
            QSharedPointer<kitty::network::object::WeatherData> dataPtr =
                    QSharedPointer<kitty::network::object::WeatherData>::create();
            in >> *dataPtr;
            emit recvWeatherMulticastMSG(dataPtr);
        }
        else
        {
            _KE("CRITICAL", "magic unknown");
        }
    }
}

void SimpleUdpServer::onStartListening()
{
    bool ret = false;
    finish_ = false;
    while (!ret && !finish_)
    {
        onStopListening();
        QHostAddress groupAddress = QHostAddress(addr_);
        if (pUdpSocket_->bind(QHostAddress::AnyIPv4, port_, QUdpSocket::ShareAddress))
        {
            QList<QNetworkInterface> mListIfaces = QNetworkInterface::allInterfaces();

            for (int i = 0; i < mListIfaces.size(); ++i)
            {
                QList<QNetworkAddressEntry> allEntries = mListIfaces.at(i).addressEntries();
                QNetworkAddressEntry entry;
                foreach (entry, allEntries)
                {
                    if (entry.ip().toString() == ifceAddr_)
                    {
                        if (pUdpSocket_->joinMulticastGroup(groupAddress, mListIfaces.at(i)))
                        {
                            ret = true;
                        }
                    }
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    connect(pUdpSocket_, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));
    isConnected_ = true;
}

void SimpleUdpServer::onStartListeningBoardcast()
{
    bool ret = false;
    finish_ = false;
    while (!ret && !finish_)
    {
        onStopListening();
        QHostAddress groupAddress = QHostAddress(addr_);
        if (pUdpSocket_->bind(QHostAddress::AnyIPv4, port_, QUdpSocket::ShareAddress))
        {
            QList<QNetworkInterface> mListIfaces = QNetworkInterface::allInterfaces();

            for (int i = 0; i < mListIfaces.size(); ++i)
            {
                QList<QNetworkAddressEntry> allEntries = mListIfaces.at(i).addressEntries();
                QNetworkAddressEntry entry;
                foreach (entry, allEntries)
                {
                    if (entry.ip().toString() == ifceAddr_)
                    {
                        if (pUdpSocket_->joinMulticastGroup(groupAddress, mListIfaces.at(i)))
                        {
                            ret = true;
                        }
                    }
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    connect(pUdpSocket_, SIGNAL(readyRead()), this, SLOT(readAndDispathPendingDatagrams()));
    isConnected_ = true;
}

void SimpleUdpServer::onStopListening()
{
    finish_ = true;
    pUdpSocket_->close();
}

void SimpleUdpServer::onSensorsMulticastMSG(QSharedPointer<kitty::network::object::SensorsMulticastMSG> msg)
{
    sendMsg(msg);
}

void SimpleUdpServer::onModbusMulticastCommand(QSharedPointer<kitty::network::object::ModbusMulticastCommand> msg)
{
    sendMsg(msg);
}

void SimpleUdpServer::onWeatherMulticastMSG(QSharedPointer<kitty::network::object::WeatherData> msg)
{
    sendMsg(msg);
}

SimpleUdpServer::~SimpleUdpServer()
{
    finish_ = true;
    pUdpSocket_->close();
}

}
}
