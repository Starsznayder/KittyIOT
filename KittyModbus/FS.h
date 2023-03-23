#ifndef FS_H
#define FS_H

#include <QObject>
#include <ModbusReader.h>
#include <fstream>

class FS : public QObject
{
    Q_OBJECT
public:
    FS();
public slots:
    void onData(ModbusData values);

private:
    int year;
    int month;
    int day;

    std::ofstream fileStream;
};

#endif // FS_H
