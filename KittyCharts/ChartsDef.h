#ifndef CHARTSDEF_H
#define CHARTSDEF_H

#include "Config.h"
#include <QString>
#include <QVector>
#include <QSharedPointer>
#include <QFuture>
#include <QtConcurrent>
#include <atomic>

class ChartsDef
{
public:
    ChartsDef(const ChartsDef&) = delete;
    ChartsDef& operator=(const ChartsDef&) = delete;

    static ChartsDef& instance()
    {
        static ChartsDef c;
        return c;
    }

    struct Def
    {
        QString name;
        int deviceIndex;
        QVector<int> registers;
    };

    QVector<int> str2regNumers(std::string value)
    {
        std::replace(value.begin(), value.end(), ',', ' ');
        std::stringstream ss(value);
        QVector<int> regs;

        std::istream_iterator<std::string> begin(ss);
        std::istream_iterator<std::string> end;
        std::vector<std::string> vstrings(begin, end);
        for (const std::string &t : vstrings)
        {
            regs.push_back(std::stoi(t));
        }
        return regs;
    }

    ConfigValue<QVector<QSharedPointer<Def>>> defs;

    ~ChartsDef()
    {
        finish.store(true);
        readerThreadObject.waitForFinished();
    }

private:   
    QFuture<void> readerThreadObject;
    std::atomic<bool> finish;

    void loadFile(const std::string& configFilepath);

    ChartsDef()
    {
        loadFile("/kitty/IOT/KittyCharts/ini/charts.ini");
        finish.store(false);
        readerThreadObject = QtConcurrent::run(this, &ChartsDef::reloadThread);
    };

    void reloadThread()
    {
        while (!finish.load())
        {
            QThread::sleep(5);
            loadFile("/kitty/IOT/KittyCharts/ini/charts.ini");
        }
    }
};

#endif // TRIGGERS_H
