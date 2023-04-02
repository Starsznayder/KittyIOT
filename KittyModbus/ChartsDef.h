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
        QVector<int> registers;
    };

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
        loadFile("ini/charts.ini");
        finish.store(false);
        readerThreadObject = QtConcurrent::run(this, &ChartsDef::reloadThread);
    };

    void reloadThread()
    {
        while (!finish.load())
        {
            QThread::sleep(5);
            loadFile("ini/charts.ini");
        }
    }
};

#endif // TRIGGERS_H
