#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <QFuture>
#include <QtConcurrent>
#include <QVector>
#include <QSharedPointer>
#include <atomic>
#include "../commons/ConfigValue.h"

class Config
{
public:
    struct DataInput
    {
        ConfigValue<std::string> ip;
        ConfigValue<unsigned> port;
    };

    struct Logs
    {
        ConfigValue<std::string> ip;
        ConfigValue<unsigned> port;
    };

    struct System
    {
        ConfigValue<unsigned> dataHistoryBufferSize;
         ConfigValue<std::string> ip;
    };

    struct Viewer
    {
        ConfigValue<bool> enable;
        ConfigValue<unsigned> port;
        ConfigValue<unsigned> timeout;
        ConfigValue<unsigned> interval;
    };

    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    static Config& instance()
    {
        static Config c;
        return c;
    }


    DataInput network;
    System systemConfig;
    Logs logs;
    Viewer viewer;

    ~Config()
    {
        finish.store(true);
        readerThreadObject.waitForFinished();
    }

private:
    QFuture<void> readerThreadObject;
    std::atomic<bool> finish;

    void loadFile(const std::string& configFilepath);

    Config()
    {
        loadFile("/kitty/IOT/KittyCharts/ini/config.ini");
        finish.store(false);
        readerThreadObject = QtConcurrent::run(this, &Config::updateThread);
    };

    void updateThread()
    {
        while (!finish.load())
        {
            QThread::sleep(5);
            loadFile("/kitty/IOT/KittyCharts/ini/config.ini");
        }
    }
};

#endif // CONFIG_H
