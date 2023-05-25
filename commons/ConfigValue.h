#ifndef CONFIGVALUE_H
#define CONFIGVALUE_H

#include <mutex>
#include <kittyLogs/log.h>

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

struct DummyBox
{
    static void showErrorBox(const QString &message)
    {
        _KE("Modbus", message.toStdString());
    }
};


#endif // CONFIGVALUE_H
