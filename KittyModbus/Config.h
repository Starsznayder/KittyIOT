#ifndef CONFIG_H
#define CONFIG_H

#include <mutex>
#include <string>
#include <QMessageBox>
#include <QFuture>
#include <QtConcurrent>
#include <atomic>

template<typename T>
class ConfigValue
{

public:
    ConfigValue(const T &value)
    {
        protector_.lock();
        value_ = value;
        protector_.unlock();
    }

    T get()
    {
        protector_.lock();
        T value = value_;
        protector_.unlock();
        return value;
    }

    void set(const T &value)
    {
        protector_.lock();
        value_ = value;
        protector_.unlock();
    }

    ConfigValue& operator=(const T& value)
    {
        protector_.lock();
        value_ = value;
        protector_.unlock();
        return *this;
    }

    ConfigValue& operator()(const T &value)
    {
        protector_.lock();
        value_ = value;
        protector_.unlock();
        return *this;
    }

    ConfigValue(){}

private:
    T value_;
    std::mutex protector_;
    ConfigValue& operator=(const ConfigValue&) = delete;
};

class DummyBox{
public:
    static void showErrorBox(const QString &message)
    {
        QMessageBox msgBox;
        msgBox.setText(message);
        msgBox.exec();
    }
};

class Config
{
public:
    struct ModbusConfig
    {
        enum class ParityCheckMethod : uint8_t
        {
            NONE = 'N',
            EVEN = 'E',
            ODD = 'O'
        };

        ConfigValue<std::string> ifaceName;
        ConfigValue<unsigned> dataAccessInterval;
        ConfigValue<unsigned> bitRate;
        ConfigValue<ParityCheckMethod> parityCheckMethod;
        ConfigValue<unsigned> dataSizeInSinglePacket;
        ConfigValue<unsigned> numStopBits;
        ConfigValue<unsigned> numRegistersPerQuery;
    };

    struct System
    {
        ConfigValue<unsigned> dataHistoryBufferSize;
    };

    class FiguresWindow
    {
    public:
        FiguresWindow(){}
        ConfigValue<QString> name;
        ConfigValue<bool> showGrid;
        ConfigValue<bool> showLegend;
        ConfigValue<unsigned> numOfCols;
        ConfigValue<unsigned> numOfRows;

    private:
        FiguresWindow& operator=(const FiguresWindow&) = delete;
        FiguresWindow(const FiguresWindow&) = delete;
    };

    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    static Config& instance()
    {
        static Config c;
        return c;
    }

    ModbusConfig modbusConfig;
    System systemConfig;
    FiguresWindow figuresWindow;

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
        loadFile("ini/config.ini");
        finish.store(false);
        readerThreadObject = QtConcurrent::run(this, &Config::updateThread);
    };

    void updateThread()
    {
        while (!finish.load())
        {
            QThread::sleep(5);
            loadFile("ini/config.ini");
        }
    }
};

#endif // CONFIG_H
