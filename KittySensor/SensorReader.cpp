#include "SensorReader.h"
#include <stdlib.h>
#include <boost/algorithm/string/replace.hpp>
#include <sstream>
#include <string>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <array>

#include <QDebug>
#include <QThread>
#include <QtConcurrent>
#include <chrono>
#include "Config.h"

std::string run(const char* cmd)
{
    std::array<char, 4096> buffer;
    std::string t;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe)
    {
        throw std::runtime_error("ERROR");
    }

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
        t += buffer.data();
    }
    return t;
}


SensorReader::SensorReader()
{
    qRegisterMetaType<SensorsData>("SensorsData");
}

void SensorReader::onDisconnect()
{
    finish.store(true);
    readerThreadObject.waitForFinished();
}
void SensorReader::onConnect()
{
    finish.store(false);
    readerThreadObject = QtConcurrent::run(this, &SensorReader::worker);
}

int reconnectSensor(const std::string& name,  const std::string& path, std::atomic<bool>* finishFlag, unsigned interval)
{
    int status = -1;
    while (!finishFlag->load() && status != 0)
    {
        status = system(path.c_str());
        if (status != 0)
        {
            _KE(name, "[INFO][Status]: INIT ERROR");
            QThread::sleep(interval);
        }
    }
    _KI(name, "[INFO][Status]: INIT OK");
    return status;
}

QSharedPointer<QVector<float>> str2sensorValues(std::string value)
{
    std::replace(value.begin(), value.end(), ',', ' ');
    std::replace(value.begin(), value.end(), '.', ',');
    std::stringstream ss(value);
    QSharedPointer<QVector<float>> out = QSharedPointer<QVector<float>>::create();

    std::istream_iterator<std::string> begin(ss);
    std::istream_iterator<std::string> end;
    std::vector<std::string> vstrings(begin, end);
    for (const std::string &t : vstrings)
    {
        out->push_back(std::stof(t));
    }
    return out;
}

QSharedPointer<QVector<float>> getValues(const std::string& name, const std::string& path)
{
    std::string result;
    try {
        result = run(path.c_str());
        _KI(name, "[INFO][Status]: READ OK");
    }
    catch (const std::runtime_error& e)
    {
        _KE(name, "[INFO][Status]: READ ERROR");
    }

    return str2sensorValues(result);
}

void SensorReader::worker()
{
    Config& config = Config::instance();
    std::vector<int> statusVector(0);

    while (!finish.load())
    {        
        for (int ic = 0; ic < config.sensorConfig.size(); ++ic)
        {
            const Config::SensorConfig mc = config.sensorConfig[ic]->get();
            if (statusVector.size() <= ic)
            {
                statusVector.push_back(reconnectSensor(mc.name,  mc.initScript, &finish, mc.dataAccessInterval));
            }
            else if (statusVector[ic] != 0)
            {
                statusVector[ic] = reconnectSensor(mc.name, mc.initScript, &finish, mc.dataAccessInterval);
            }

            if (statusVector[ic] == 0)
            {
                QSharedPointer<QVector<float>> vals = getValues(mc.name, mc.readScript);

                if (vals->size() > 0)
                {
                    SensorsData output;
                    const auto p1 = std::chrono::system_clock::now();
                    output.timestamp = std::chrono::duration_cast<std::chrono::microseconds>(p1.time_since_epoch()).count();
                    output.values = vals;
                    output.devIndex = mc.deviceIndex;

                    emit freshData(output);
                    QSharedPointer<kitty::network::object::SensorsMulticastMSG> mmmsg =
                            QSharedPointer<kitty::network::object::SensorsMulticastMSG>::create(*output.values, output.timestamp, mc.deviceIndex);
                    emit sensorsMulticastMSG(mmmsg);
                }
                else
                {
                    _KE(mc.name, "[INFO][Status]: Empty result");
                }
            }
            QThread::sleep(mc.dataAccessInterval);
        }
    }
}
