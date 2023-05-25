#ifndef CONFIG_H
#define CONFIG_H

#include <mutex>
#include <string>
#include <QMessageBox>
#include <QFuture>
#include <QtConcurrent>
#include <atomic>
#include <kittyLogs/log.h>
#include "../commons/ConfigValue.h"

class Config
{
public:
    struct Logs
    {
        ConfigValue<std::string> ip;
        ConfigValue<unsigned> port;
    };

    struct System
    {
        ConfigValue<std::string> dataFilePath;
        ConfigValue<std::string> ip;
        ConfigValue<bool> logDataToFile;
    };

    struct Network
    {
        ConfigValue<std::string> dataServerAddr;
        ConfigValue<unsigned> interval;
    };

    struct DataOutput
    {
        ConfigValue<std::string> ip;
        ConfigValue<unsigned> port;
    };


    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    static Config& instance()
    {
        static Config c;
        return c;
    }

    System systemConfig;
    Logs logs;
    Network dataWebsite;
    DataOutput network;

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
        loadFile("/kitty/IOT/Weather/ini/weatherConfig.ini");
        finish.store(false);
        readerThreadObject = QtConcurrent::run(this, &Config::updateThread);
    };

    void updateThread()
    {
        while (!finish.load())
        {
            QThread::sleep(5);
            loadFile("/kitty/IOT/Weather/ini/weatherConfig.ini");
        }
    }
};

#endif // CONFIG_H
