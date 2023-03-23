#include "Triggers.h"
#include <QMessageBox>
#include <QDebug>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/algorithm/string/replace.hpp>

#include <sstream>
#include <string>


using ptree = boost::property_tree::ptree;
namespace property_tree = boost::property_tree;
namespace ini_parser = boost::property_tree::ini_parser;

QSharedPointer<operators::MultiregOperator> str2operator(const std::string& value, const QVector<int>& registers)
{
    if (value == std::string("SUM"))
    {
        return QSharedPointer<operators::Sum>::create(registers);
    }
    else
    {
        return QSharedPointer<operators::Average>::create(registers);
    }
}

Triggers::Trigger::ThresholdType str2thresholdType(const std::string& value)
{
    if (value == std::string("LO"))
    {
        return Triggers::Trigger::ThresholdType::LO;
    }
    else
    {
        return Triggers::Trigger::ThresholdType::GT;
    }
}

QSharedPointer<filters::Filter> str2filter(const std::string& value, int order)
{
    if (value == std::string("MEDIAN"))
    {
        return QSharedPointer<filters::Median>::create(order);
    }
    else if (value == std::string("MEAN"))
    {
        return QSharedPointer<filters::Mean>::create(order);
    }
    else if (value == std::string("MIN"))
    {
        return QSharedPointer<filters::Min>::create(order);
    }
    else
    {
        return QSharedPointer<filters::Max>::create(order);
    }
}

QVector<int> Triggers::str2regNumers(std::string value)
{
    std::replace(value.begin(), value.end(), ',', ' ');
    std::stringstream ss(value);
    QVector<int> regs;

    std::istream_iterator<std::string> begin(ss);
    std::istream_iterator<std::string> end;
    std::vector<std::string> vstrings(begin, end);
    for (const std::string &t : vstrings)
    {
        regs.push_back(std::stoi(t));
    }
    return regs;
}

void Triggers::loadFile(const std::string& configFilepath)
{
    ptree pt;
    std::string prefix("Trigger");
    bool gotAll = false;
    int index = 0;

    try{
        qDebug() << configFilepath.c_str();
        ini_parser::read_ini(configFilepath, pt);
        while (!gotAll)
        {
            try {
                std::string header = prefix + std::to_string(index);
                triggers.push_back(QSharedPointer<ConfigValue<Trigger>>::create(Trigger({str2regNumers(pt.get<std::string>(header + ".registers")),
                                                                                         str2operator(pt.get<std::string>(header + ".operator"), str2regNumers(pt.get<std::string>(header + ".registers"))),
                                                                                         str2filter(pt.get<std::string>(header + ".filter"), pt.get<int>(header + ".order")),
                                                                                         pt.get<int>(header + ".order"),
                                                                                         pt.get<float>(header + ".turnOnThreshold"),
                                                                                         str2thresholdType(pt.get<std::string>(header + ".turnOnThresholdType")),
                                                                                         QString::fromStdString(pt.get<std::string>(header + ".turnOnRunPath")),
                                                                                         pt.get<float>(header + ".turnOffThreshold"),
                                                                                         str2thresholdType(pt.get<std::string>(header + ".turnOffThresholdType")),
                                                                                         QString::fromStdString(pt.get<std::string>(header + ".turnOffRunPath")),
                                                                                         pt.get<int>(header + ".minRunTimePerDay"),
                                                                                         pt.get<int>(header + ".runTimeFulfillmentTimeBorder")})));
            }
            catch (const property_tree::ptree_bad_path&)
            {
                gotAll = true;
            }
            catch (const property_tree::ptree_bad_data&)
            {
                DummyBox::showErrorBox("Triger definition is broken");
            }
            ++index;
        }
    }
    catch (const ini_parser::ini_parser_error&)
    {
        DummyBox::showErrorBox("Triger definition is broken");
    }
}
