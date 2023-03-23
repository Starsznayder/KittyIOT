#ifndef DATABUFER_H
#define DATABUFER_H

#include <QObject>
#include "FigureWindow/FigureData.h"
#include "ModbusReader.h"

class DataBufer : public QObject
{
    Q_OBJECT
public:
    DataBufer();
signals:
    void gotData(QSharedPointer<kitty::network::object::FigureData>);
public slots:
    void onData(ModbusData values);

private:
QVector<QVector<float>> data;
QVector<uint64_t> timestamps;

};

#endif // DATABUFER_H
