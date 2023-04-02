#include "Process.h"
#include <stdlib.h>

Process::Process(int _triggerIndex) : triggerIndex(_triggerIndex), updateCounter(0)
{
    isAlreadyOn = false;
    Triggers::Trigger trigger = Triggers::instance().triggers[triggerIndex]->get();

    int status = system(trigger.turnOffRunPath.toStdString().c_str());

    if (status != 0)
    {
        DummyBox::showErrorBox(trigger.turnOffRunPath + " exit code != 0");
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
   // qDebug() << "currentValue:" << currentValue;
    ++updateCounter;
   // qDebug() << "updateCounter: " << updateCounter;

    if (day != timestamp2day(values.timestamp))
    {
        runtimeCounter = 0;
        day = timestamp2day(values.timestamp);
    }
   // qDebug() << "day: " << timestamp2day(values.timestamp);

    if (updateCounter >= trigger.order)
    {
       // qDebug() << "updateCounter >= trigger.order";
       // qDebug() << "timestamp2hour(timestamp): " << timestamp2hour(values.timestamp);
        // qDebug() << "timerCondition(values.timestamp): " << timerCondition(values.timestamp);
        if (isAlreadyOn)
        {
            runtimeCounter += static_cast<double>(values.timestamp - turnOnTimestamp) / 1e6;
            turnOnTimestamp = values.timestamp;
          //  qDebug() << "runtimeCounter: " << runtimeCounter;
        }

        if (threshold(currentValue, trigger.turnOnThreshold, trigger.turnOnThresholdType) && !isAlreadyOn)
        {
            qDebug() << "turn on: timestamp2hour(timestamp): " << timestamp2hour(values.timestamp);
             qDebug() << "runtimeCounter: " << runtimeCounter;
            isAlreadyOn = true;
            int status = system(trigger.turnOnRunPath.toStdString().c_str());
            turnOnTimestamp = values.timestamp;
            if (status != 0)
            {
                DummyBox::showErrorBox(trigger.turnOnRunPath + " exit code != 0");
            }
        }
        else if (threshold(currentValue, trigger.turnOffThreshold, trigger.turnOffThresholdType) && isAlreadyOn && !timerCondition(values.timestamp, trigger))
        {
            qDebug() << "turn off: timestamp2hour(timestamp): " << timestamp2hour(values.timestamp);
            qDebug() << "runtimeCounter: " << runtimeCounter;
            isAlreadyOn = false;
            int status = system(trigger.turnOffRunPath.toStdString().c_str());

            if (status != 0)
            {
                DummyBox::showErrorBox(trigger.turnOffRunPath + " exit code != 0");
            }
        }
        else if (!isAlreadyOn && timerCondition(values.timestamp, trigger))
        {
            qDebug() << "turn on: timestamp2hour(timestamp): " << timestamp2hour(values.timestamp);
            qDebug() << "runtimeCounter: " << runtimeCounter;
            isAlreadyOn = true;
            int status = system(trigger.turnOnRunPath.toStdString().c_str());
            turnOnTimestamp = values.timestamp;
            if (status != 0)
            {
                DummyBox::showErrorBox(trigger.turnOnRunPath + " exit code != 0");
            }
        }
    }
}
