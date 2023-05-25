#ifndef FS_H
#define FS_H

#include <QObject>
#include <fstream>
#include "MuticastMessageParser.h"

class FS : public QObject
{
    Q_OBJECT
public:
    FS();
public slots:
    void onData(SensorsData values);
    void onData(QSharedPointer<kitty::network::object::WeatherData>);

private:
    int day;
    int month;
    int year;

    std::ofstream fileStream;
};

#endif // FS_H
