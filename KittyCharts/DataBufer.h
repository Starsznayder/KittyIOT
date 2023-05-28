#ifndef DATABUFER_H
#define DATABUFER_H

#include <QObject>
#include "KittyNetwork/FigureData.h"
#include "KittyNetwork/KittyNetwork.h"
#include "MuticastMessageParser.h"
#include "SimpleUdpServer.h"

class DataBufer : public QObject
{
    Q_OBJECT
public:
    DataBufer(std::string ip, unsigned port, unsigned timeout);
signals:
    void gotData(QSharedPointer<kitty::network::object::FigureData>);
public slots:
    void onData(SensorsData values);

private:
    KittyNetwork net_;
    QVector<QVector<QVector<float>>> data;
    QVector<QVector<uint64_t>> timestamps;

};

#endif // DATABUFER_H
