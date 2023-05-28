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
        ConfigValue<std::string> ip;
        ConfigValue<std::string> weatherFilePath;
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
        loadFile("/kitty/IOT/KittyDevice/ini/config.ini");
        finish.store(false);
        readerThreadObject = QtConcurrent::run(this, &Config::updateThread);
    };

    void updateThread()
    {
        while (!finish.load())
        {
            QThread::sleep(5);
            loadFile("/kitty/IOT/KittyDevice/ini/config.ini");
        }
    }
};

#endif // CONFIG_H
