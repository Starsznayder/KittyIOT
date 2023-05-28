#include "Process.h"
#include "Config.h"
#include <stdlib.h>

void errorHandler(int status, std::string pName, std::string ops)
{
    if (status != 0)
    {
        _KE(pName, "[INFO][Status]: " << ops);
        DummyBox::showErrorBox(QString::fromStdString(pName) + " exit code != 0");
    }
}

Process::Process(int _triggerIndex) : triggerIndex(_triggerIndex), updateCounter(0), isAlreadyOn(false)
{

}

void Process::onRun()
{
    Triggers::Trigger trigger = Triggers::instance().triggers[triggerIndex]->get();
    _KE(trigger.name.toStdString(), "[INFO][Status]: STOP");
    turnOff(trigger);
}

void Process::onRunBySun()
{
    if (sunTrigger->get().thresholdType == SunTrigger::Trigger::ThresholdType::NIGHT ||
        sunTrigger->get().thresholdType == SunTrigger::Trigger::ThresholdType::DAY)
    {
        std::string action = sunTrigger->get().actionPath.toStdString().c_str() + std::string(" ") +
                             std::to_string(sunTrigger->get().delay) + std::string(" ") + std::to_string(0);
        turnOffBySun(action);
        _KE(sunTrigger->get().name.toStdString(), "[INFO][Status]: STOP");
    }
}

bool threshold(float value, float ref, Triggers::Trigger::ThresholdType tt)
{
    if (tt == Triggers::Trigger::ThresholdType::GT)
    {
        return value >= ref;
    }
    else if (tt == Triggers::Trigger::ThresholdType::LO)
    {
        return value <= ref;
    }

    return false;
}

int timestamp2day(uint64_t timestamp)
{
    std::chrono::system_clock::time_point uptime_timepoint{std::chrono::duration_cast<std::chrono::system_clock::time_point::duration>(std::chrono::microseconds(timestamp))};
    std::time_t unix_timestamp = std::chrono::system_clock::to_time_t(uptime_timepoint);
    struct tm ts = *localtime(&unix_timestamp);
    return ts.tm_mday;
}

int timestamp2hour(uint64_t timestamp)
{
    std::chrono::system_clock::time_point uptime_timepoint{std::chrono::duration_cast<std::chrono::system_clock::time_point::duration>(std::chrono::microseconds(timestamp))};
    std::time_t unix_timestamp = std::chrono::system_clock::to_time_t(uptime_timepoint);
    struct tm ts = *localtime(&unix_timestamp);
    return ts.tm_hour;
}

int timestamp2minute(uint64_t timestamp)
{
    std::chrono::system_clock::time_point uptime_timepoint{std::chrono::duration_cast<std::chrono::system_clock::time_point::duration>(std::chrono::microseconds(timestamp))};
    std::time_t unix_timestamp = std::chrono::system_clock::to_time_t(uptime_timepoint);
    struct tm ts = *localtime(&unix_timestamp);
    return ts.tm_min;
}

bool Process::timerCondition(uint64_t timestamp, const  Triggers::Trigger& trigger)
{
    if (timestamp2hour(timestamp) >= trigger.runTimeFulfillmentTimeBorder && runtimeCounter < trigger.minRunTimePerDay)
    {
        return true;
    }
    return false;
}

void Process::turnOn(const Triggers::Trigger& trigger)
{
    if (trigger.mode == Triggers::Trigger::Mode::SCRIPT)
    {
        errorHandler(system(trigger.turnOnRunPath.toStdString().c_str()), trigger.name.toStdString(), "turn on error");
    }
    else
    {
        QSharedPointer<kitty::network::object::ModbusMulticastCommand> com =
                QSharedPointer<kitty::network::object::ModbusMulticastCommand>::create(trigger.modbusDevID, trigger.onOffRegisterAddr, 1);
        emit command(com);
    }
}

void Process::turnOff(const Triggers::Trigger& trigger)
{
    if (trigger.mode == Triggers::Trigger::Mode::SCRIPT)
    {
        errorHandler(system(trigger.turnOffRunPath.toStdString().c_str()), trigger.name.toStdString(), "turn off error");
    }
    else
    {
        QSharedPointer<kitty::network::object::ModbusMulticastCommand> com =
                QSharedPointer<kitty::network::object::ModbusMulticastCommand>::create(trigger.modbusDevID, trigger.onOffRegisterAddr, 0);
        emit command(com);
    }
}

void Process::onData(SensorsData values)
{
    Triggers::Trigger trigger = Triggers::instance().triggers[triggerIndex]->get();

    if (values.devIndex != trigger.srcDeviceIndex)
    {
        return;
    }

    float currentValue = trigger.filter->execute(trigger.multiregOperator->execute(values));
    ++updateCounter;

    if (day != timestamp2day(values.timestamp))
    {
        runtimeCounter = 0;
        day = timestamp2day(values.timestamp);
    }

    if (updateCounter >= trigger.order)
    {
        if (isAlreadyOn)
        {
            runtimeCounter += static_cast<double>(values.timestamp - turnOnTimestamp) / 1e6;
            turnOnTimestamp = values.timestamp;
            if (trigger.manualMode && trigger.oN)
            {
                _KW(trigger.name.toStdString(), "[INFO][Status]: RUN (manual)");
            }
            else if(timerCondition(values.timestamp, trigger))
            {
                _KW(trigger.name.toStdString(), "[INFO][Status]: RUN (timer condition)");
            }
            else
            {
                _KI(trigger.name.toStdString(), "[INFO][Status]: RUN");
            }
        }
        else
        {
            if (trigger.manualMode && !trigger.oN)
            {
                _KW(trigger.name.toStdString(), "[INFO][Status]: STOP (manual)");
            }
            else
            {
                _KI(trigger.name.toStdString(), "[INFO][Status]: STOP");
            }
        }

        _KI(trigger.name.toStdString(), "[INFO][RUNTIME]: " << runtimeCounter);
        _KI(trigger.name.toStdString(), "[INFO][VALUE]: " << currentValue);

        if (trigger.manualMode)
        {
            if (trigger.oN && !isAlreadyOn)
            {
                _KW(trigger.name.toStdString(), "[INFO][Status]: RUN (manual)");
                isAlreadyOn = true;
                turnOn(trigger);
                turnOnTimestamp = values.timestamp;
            }
            else if (!trigger.oN && isAlreadyOn)
            {
                _KW(trigger.name.toStdString(), "[INFO][Status]: STOP (manual)");
                isAlreadyOn = false;
                turnOff(trigger);
                turnOnTimestamp = values.timestamp;
            }
        }
        else
        {
            if (threshold(currentValue, trigger.turnOnThreshold, trigger.turnOnThresholdType) && !isAlreadyOn)
            {
                _KI(trigger.name.toStdString(), "[INFO][Status]: RUN");
                isAlreadyOn = true;
                turnOn(trigger);
                turnOnTimestamp = values.timestamp;
            }
            else if (threshold(currentValue, trigger.turnOffThreshold, trigger.turnOffThresholdType) && isAlreadyOn && !timerCondition(values.timestamp, trigger))
            {
                _KI(trigger.name.toStdString(), "[INFO][Status]: STOP");
                isAlreadyOn = false;
                turnOff(trigger);
            }
            else if (!isAlreadyOn && timerCondition(values.timestamp, trigger))
            {
                _KW(trigger.name.toStdString(), "[INFO][Status]: RUN (timer condition)");
                isAlreadyOn = true;
                turnOn(trigger);
                turnOnTimestamp = values.timestamp;
            }
        }
    }
}

void Process::turnOnBySun(const std::string& action)
{
    if (sunTrigger->get().mode == SunTrigger::Trigger::Mode::SCRIPT)
    {
         errorHandler(system(action.c_str()), sunTrigger->get().name.toStdString(), "run error");
    }
    else
    {
        QSharedPointer<kitty::network::object::ModbusMulticastCommand> com =
                QSharedPointer<kitty::network::object::ModbusMulticastCommand>::create(sunTrigger->get().modbusDevID, sunTrigger->get().onOffRegisterAddr, 1);
        emit command(com);
    }
}

void Process::turnOffBySun(const std::string& action)
{
    if (sunTrigger->get().mode == SunTrigger::Trigger::Mode::SCRIPT)
    {
        errorHandler(system(action.c_str()), sunTrigger->get().name.toStdString(), "run error");
    }
    else
    {
        QSharedPointer<kitty::network::object::ModbusMulticastCommand> com =
                QSharedPointer<kitty::network::object::ModbusMulticastCommand>::create(sunTrigger->get().modbusDevID, sunTrigger->get().onOffRegisterAddr, 0);
        emit command(com);
    }
}

void Process::onData(kitty::network::object::Sun values)
{
    if (!sunTrigger)
    {
        return;
    }

    values.riseTimestamp  /= static_cast<uint64_t>(1000000);
    values.setTimestamp  /= static_cast<uint64_t>(1000000);
    const int64_t riseHour = timestamp2hour(values.riseTimestamp * static_cast<uint64_t>(1000000));
    const int64_t riseMinute = timestamp2minute(values.riseTimestamp * static_cast<uint64_t>(1000000));
    const int64_t fallHour = timestamp2hour(values.setTimestamp * static_cast<uint64_t>(1000000));
    const int64_t fallMinute = timestamp2minute(values.setTimestamp * static_cast<uint64_t>(1000000));
    _KI(sunTrigger->get().name.toStdString(), "[INFO][Sunrise]: " << riseHour << ":" << riseMinute);
    _KI(sunTrigger->get().name.toStdString(), "[INFO][Sunset]: " << fallHour << ":" << fallMinute);

    const auto p1 = std::chrono::system_clock::now();
    uint64_t curentTimestamp = std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch()).count();

    const int64_t minute = timestamp2minute(curentTimestamp * static_cast<uint64_t>(1000000));
    const int64_t hour = timestamp2hour(curentTimestamp * static_cast<uint64_t>(1000000));
    _KI(sunTrigger->get().name.toStdString(), "[INFO][Time]: " << hour << ":" << minute);

    if (day != timestamp2day(curentTimestamp))
    {
        day = timestamp2day(curentTimestamp);
        isAlreadyOn = false;
    }

    int64_t nightLen = (riseHour * static_cast<int64_t>(3600) + riseMinute * static_cast<int64_t>(60) + static_cast<int64_t>(24 * 3600)) -
                        (fallHour * static_cast<int64_t>(3600) + fallMinute * static_cast<int64_t>(60));
    int64_t dayLen = (fallHour * static_cast<int64_t>(3600) + fallMinute * static_cast<int64_t>(60)) -
                      (riseHour * static_cast<int64_t>(3600) + riseMinute * static_cast<int64_t>(60));

    _KI(sunTrigger->get().name.toStdString(), "[INFO][NightLength]: " << nightLen / 3600 << ":" << (nightLen - (nightLen / 3600) * 3600) / 60);
    _KI(sunTrigger->get().name.toStdString(), "[INFO][DaytLength]: " << dayLen / 3600 << ":" << (dayLen - (dayLen / 3600) * 3600) / 60);

    if (isAlreadyOn)
    {
        _KI(sunTrigger->get().name.toStdString(), "[INFO][Status]: RUN");
    }
    else
    {
        _KI(sunTrigger->get().name.toStdString(), "[INFO][Status]: STOP");
    }

    if (!sunTrigger->get().manualMode)
    {
        if (sunTrigger->get().thresholdType == SunTrigger::Trigger::ThresholdType::FALL &&
            (curentTimestamp >= values.setTimestamp - sunTrigger->get().offset  || curentTimestamp <= values.riseTimestamp) &&
            !isAlreadyOn)
        {
            int64_t dT = (riseHour * static_cast<int64_t>(3600) + riseMinute * static_cast<int64_t>(60) +
                          static_cast<int64_t>(24 * 3600) + sunTrigger->get().offset) -
                         (hour * static_cast<int64_t>(3600) + minute * static_cast<int64_t>(60));

            while(dT >= 24 * 3600)
            {
                dT -= 24 * 3600;
            }

            _KW(sunTrigger->get().name.toStdString(), "[INFO][dT]: " << dT);
            if (dT > 0)
            {
                std::string action = sunTrigger->get().actionPath.toStdString().c_str() + std::string(" ") +
                                     std::to_string(sunTrigger->get().delay) + std::string(" ") + std::to_string(dT);
                turnOnBySun(action);
                isAlreadyOn = true;
                _KW(sunTrigger->get().name.toStdString(), "[INFO][Run]: " << hour << ":" << minute);

            }
            else
            {
                _KE(sunTrigger->get().name.toStdString(), "dT < 0!");
            }
        }
        else if (sunTrigger->get().thresholdType == SunTrigger::Trigger::ThresholdType::RISE &&
                 curentTimestamp > values.riseTimestamp + sunTrigger->get().offset &&
                 !isAlreadyOn)
        {
            int64_t dT = (fallHour * static_cast<int64_t>(3600) + fallMinute * static_cast<int64_t>(60) - sunTrigger->get().offset) -
                         (hour * static_cast<int64_t>(3600) + minute * static_cast<int64_t>(60));

            while(dT >= 24*3600)
            {
                dT -= 24*3600;
            }

            _KW(sunTrigger->get().name.toStdString(), "[INFO][dT]: " << dT);
            if (dT > 0)
            {
                std::string action = sunTrigger->get().actionPath.toStdString().c_str() + std::string(" ") +
                                     std::to_string(sunTrigger->get().delay) + std::string(" ") + std::to_string(dT);
                turnOnBySun(action);
                isAlreadyOn = true;
                _KI(sunTrigger->get().name.toStdString(), "[INFO][Run]: " << hour << ":" << minute);
            }
            else
            {
                _KE(sunTrigger->get().name.toStdString(), "dT < 0!");
            }
        }
        else if (sunTrigger->get().thresholdType == SunTrigger::Trigger::ThresholdType::NIGHT &&
                 ((hour * 3600 + minute * 60) + sunTrigger->get().offset > fallHour * 3600 + fallMinute * 60 ||
                  (hour * 3600 + minute * 60) - sunTrigger->get().offset < riseHour * 3600 + riseMinute * 60))
        {

                std::string action = sunTrigger->get().actionPath.toStdString().c_str() + std::string(" ") +
                                     std::to_string(sunTrigger->get().delay) + std::string(" ") + std::to_string(0);
                turnOnBySun(action);
                isAlreadyOn = true;
                _KI(sunTrigger->get().name.toStdString(), "[INFO][Run]: " << hour << ":" << minute);

        }
        else if (sunTrigger->get().thresholdType == SunTrigger::Trigger::ThresholdType::NIGHT &&
                 ((hour * 3600 + minute * 60) + sunTrigger->get().offset < fallHour * 3600 + fallMinute * 60) &&
                 ((hour * 3600 + minute * 60) - sunTrigger->get().offset > riseHour * 3600 + riseMinute * 60))
        {
                std::string action = sunTrigger->get().actionPath.toStdString().c_str() + std::string(" ") +
                                     std::to_string(sunTrigger->get().delay) + std::string(" ") + std::to_string(0);
                turnOffBySun(action);
                isAlreadyOn = false;
                _KI(sunTrigger->get().name.toStdString(), "[INFO][Stop]: " << hour << ":" << minute);
        }
        else
        {
            _KI(sunTrigger->get().name.toStdString(), "[INFO][Run]: waiting...");
        }
    }
    else
    {
        _KW(sunTrigger->get().name.toStdString(), "[INFO][Run]: manual mode");
    }
}
