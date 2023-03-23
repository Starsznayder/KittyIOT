#include "FS.h"
#include <boost/filesystem.hpp>
#include <chrono>
#include <memory>
#include <QDebug>
#include <Config.h>

std::shared_ptr<std::string> getHumanizedTime(uint64_t timestamp, int& dd, int& mm, int& yyyy)
{
    std::chrono::system_clock::time_point uptime_timepoint{std::chrono::duration_cast<std::chrono::system_clock::time_point::duration>(std::chrono::microseconds(timestamp))};
    std::time_t unix_timestamp = std::chrono::system_clock::to_time_t(uptime_timepoint);
    char buffer[50];
    struct tm ts = *localtime(&unix_timestamp);
    yyyy = ts.tm_year + 1900;
    mm = ts.tm_mon + 1;
    dd = ts.tm_mday;
    strftime(buffer, 50, "%Y_%m_%d\0", &ts);
    std::shared_ptr<std::string> timeString = std::make_shared<std::string>(buffer);
    return timeString;
}

FS::FS() : day(0), month(0), year(0)
{
    boost::filesystem::create_directories("data");
}

void FS::onData(ModbusData values)
{
    int dd, mm, yyyy;
    std::shared_ptr<std::string> timeString = getHumanizedTime(values.timestamp, dd, mm, yyyy);
    //qDebug() <<  dd << "-" << mm << "-" << yyyy;
   // qDebug() << QString::fromStdString(*timeString);

    if (dd != day || mm != month || yyyy != year)
    {
        day = dd;
        month =mm;
        year = yyyy;
        if (fileStream.is_open())
        {
            fileStream.close();
        }

        fileStream.open(std::string("data/") + *timeString + ".kmdb", std::ios::binary);
    }

    if (!fileStream.good())
    {
        DummyBox::showErrorBox("Unable to write data file!");
    }
    else
    {
        try{
            fileStream.write(reinterpret_cast<char*>(&values.timestamp), sizeof(uint64_t));
            fileStream.write(reinterpret_cast<char*>(values.values->data()), values.values->size() * sizeof(float));
            fileStream.flush();
        }
        catch (const std::exception& e)
        {
            DummyBox::showErrorBox(QString::fromStdString(e.what()));
        }
    }
}
