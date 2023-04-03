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

Process::Process(int _triggerIndex) : triggerIndex(_triggerIndex), updateCounter(0)
{
    isAlreadyOn = false;
    Triggers::Trigger trigger = Triggers::instance().triggers[triggerIndex]->get();
    errorHandler(system(trigger.turnOffRunPath.toStdString().c_str()), trigger.name.toStdString(), "turn off error");
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

bool Process::timerCondition(uint64_t timestamp, const  Triggers::Trigger& trigger)
{
    if (timestamp2hour(timestamp) >= trigger.runTimeFulfillmentTimeBorder && runtimeCounter < trigger.minRunTimePerDay)
    {
        return true;
    }
    return false;
}

void Process::onData(ModbusData values)
{
    Triggers::Trigger trigger = Triggers::instance().triggers[triggerIndex]->get();

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
            _KI(trigger.name.toStdString(), "[INFO][RUNTIME]: " << runtimeCounter);
        }

        if (threshold(currentValue, trigger.turnOnThreshold, trigger.turnOnThresholdType) && !isAlreadyOn)
        {
            _KI(trigger.name.toStdString(), "[INFO][Status]: RUN");
            isAlreadyOn = true;
            errorHandler(system(trigger.turnOnRunPath.toStdString().c_str()), trigger.name.toStdString(), "turn on error");
            turnOnTimestamp = values.timestamp;
        }
        else if (threshold(currentValue, trigger.turnOffThreshold, trigger.turnOffThresholdType) && isAlreadyOn && !timerCondition(values.timestamp, trigger))
        {
            _KE(trigger.name.toStdString(), "[INFO][Status]: STOP");
            isAlreadyOn = false;
            errorHandler(system(trigger.turnOffRunPath.toStdString().c_str()), trigger.name.toStdString(), "turn off error");
        }
        else if (!isAlreadyOn && timerCondition(values.timestamp, trigger))
        {
            _KW(trigger.name.toStdString(), "[INFO][Status]: RUN (timer condition)");
            isAlreadyOn = true;
            errorHandler(system(trigger.turnOnRunPath.toStdString().c_str()), trigger.name.toStdString(), "turn on error");
            turnOnTimestamp = values.timestamp;
        }
    }
}
