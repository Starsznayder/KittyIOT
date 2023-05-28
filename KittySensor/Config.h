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
    struct SensorConfig
    {
        std::string name;
        std::string initScript;
        std::string readScript;
        unsigned dataAccessInterval;
        unsigned deviceIndex;
    };

    struct DataOutput
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
        ConfigValue<std::string> dataFilePath;
        ConfigValue<bool> logDataToFile;
        ConfigValue<std::string> ip;
    };

    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    static Config& instance()
    {
        static Config c;
        return c;
    }

    QVector<QSharedPointer<ConfigValue<SensorConfig>>> sensorConfig;
    DataOutput network;
    System systemConfig;
    Logs logs;

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
        loadFile("/kitty/IOT/KittySensor/ini/config.ini");
        finish.store(false);
        readerThreadObject = QtConcurrent::run(this, &Config::updateThread);
    };

    void updateThread()
    {
        while (!finish.load())
        {
            QThread::sleep(5);
            loadFile("/kitty/IOT/KittySensor/ini/config.ini");
        }
    }
};

#endif // CONFIG_H
