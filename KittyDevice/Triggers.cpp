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

operators::OperatorType str2operatorType(const std::string& value)
{
    if (value == std::string("SUM"))
    {
        return operators::OperatorType::SUM;
    }
    else
    {
        return operators::OperatorType::AVERAGE;
    }
}


QSharedPointer<operators::MultiregOperator> createOperator(operators::OperatorType ot, const QVector<int>& registers)
{
    if (ot == operators::OperatorType::SUM)
    {
        return QSharedPointer<operators::Sum>::create(registers);
    }
    else
    {
        return QSharedPointer<operators::Average>::create(registers);
    }
}

Triggers::Trigger::ThresholdType Triggers::str2thresholdType(const std::string& value)
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

Triggers::Trigger::Mode Triggers::str2mode(const std::string& value)
{
    if (value == std::string("MODBUS"))
    {
        return Triggers::Trigger::Mode::MODBUS;
    }
    else
    {
        return Triggers::Trigger::Mode::SCRIPT;
    }
}

filters::FilterType str2filterType(const std::string& value)
{
    if (value == std::string("MEDIAN"))
    {
        return filters::FilterType::MEDIAN;
    }
    else if (value == std::string("MEAN"))
    {
        return filters::FilterType::MEAN;
    }
    else if (value == std::string("MIN"))
    {
        return filters::FilterType::MIN;
    }
    else
    {
        return filters::FilterType::MAX;
    }
}

QSharedPointer<filters::Filter> createFilter(filters::FilterType ft, int order)
{
    if (ft == filters::FilterType::MEDIAN)
    {
        return QSharedPointer<filters::Median>::create(order);
    }
    else if (ft == filters::FilterType::MEAN)
    {
        return QSharedPointer<filters::Mean>::create(order);
    }
    else if (ft == filters::FilterType::MIN)
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

void Triggers::Trigger::update(QString _name,
                               Mode _mode,
                               unsigned _onOffRegisterAddr,
                               unsigned _modbusDevID,
                               int _srcDeviceIndex,
                               QVector<int> _registers,
                               filters::FilterType filterType,
                               operators::OperatorType operatorType,
                               int _order,
                               float _turnOnThreshold,
                               ThresholdType _turnOnThresholdType,
                               QString _turnOnRunPath,
                               float _turnOffThreshold,
                               ThresholdType _turnOffThresholdType,
                               QString _turnOffRunPath,
                               int _minRunTimePerDay,
                               int _runTimeFulfillmentTimeBorder,
                               bool _manualMode,
                               bool _oN)
{
    name = _name;
    mode = _mode;
    onOffRegisterAddr = _onOffRegisterAddr;
    modbusDevID = _modbusDevID;
    srcDeviceIndex = _srcDeviceIndex;
    order = _order;
    turnOnThreshold = _turnOnThreshold;
    turnOnThresholdType = _turnOnThresholdType;
    turnOnRunPath = _turnOnRunPath;
    turnOffThreshold = _turnOffThreshold;
    turnOffThresholdType = _turnOffThresholdType;
    turnOffRunPath = _turnOffRunPath;
    minRunTimePerDay = _minRunTimePerDay;
    runTimeFulfillmentTimeBorder = _runTimeFulfillmentTimeBorder;
    manualMode = _manualMode;
    oN = _oN;

    if (!filter->isCompatible(_order, filterType))
    {
        filter = createFilter(filterType, _order);
    }

    if (!multiregOperator->isCompatible(operatorType, _registers))
    {
        registers = _registers;
        multiregOperator = createOperator(operatorType, _registers);
    }
}

void Triggers::loadFile(const std::string& configFilepath)
{
    ptree pt;
    std::string prefix("Trigger");
    bool gotAll = false;
    int index = 0;

    try{
        ini_parser::read_ini(configFilepath, pt);
        while (!gotAll)
        {
            try {
                std::string header = prefix + std::to_string(index);
                if (index >= triggers.size())
                {
                    triggers.push_back(QSharedPointer<ConfigValue<Trigger>>::create(Trigger({QString::fromStdString(pt.get<std::string>(header + ".name")),
                                                                                             str2mode(pt.get<std::string>(header + ".mode")),
                                                                                             pt.get<unsigned>(header + ".onOffRegisterAddr"),
                                                                                             pt.get<unsigned>(header + ".modbusDevID"),
                                                                                             pt.get<int>(header + ".srcDeviceIndex"),
                                                                                             str2regNumers(pt.get<std::string>(header + ".registers")),
                                                                                             createOperator(str2operatorType(pt.get<std::string>(header + ".operator")),
                                                                                                            str2regNumers(pt.get<std::string>(header + ".registers"))),
                                                                                             createFilter(str2filterType(pt.get<std::string>(header + ".filter")),
                                                                                                          pt.get<int>(header + ".order")),
                                                                                             pt.get<int>(header + ".order"),
                                                                                             pt.get<float>(header + ".turnOnThreshold"),
                                                                                             str2thresholdType(pt.get<std::string>(header + ".turnOnThresholdType")),
                                                                                             QString::fromStdString(pt.get<std::string>(header + ".turnOnRunPath")),
                                                                                             pt.get<float>(header + ".turnOffThreshold"),
                                                                                             str2thresholdType(pt.get<std::string>(header + ".turnOffThresholdType")),
                                                                                             QString::fromStdString(pt.get<std::string>(header + ".turnOffRunPath")),
                                                                                             pt.get<int>(header + ".minRunTimePerDay"),
                                                                                             pt.get<int>(header + ".runTimeFulfillmentTimeBorder"),
                                                                                             pt.get<bool>(header + ".manualMode"),
                                                                                             pt.get<bool>(header + ".ON")})));
                }
                else
                {
                    Trigger t = triggers[index]->get();
                    t.update(QString::fromStdString(pt.get<std::string>(header + ".name")),
                             str2mode(pt.get<std::string>(header + ".mode")),
                             pt.get<unsigned>(header + ".onOffRegisterAddr"),
                             pt.get<unsigned>(header + ".modbusDevID"),
                             pt.get<int>(header + ".srcDeviceIndex"),
                             str2regNumers(pt.get<std::string>(header + ".registers")),
                             str2filterType(pt.get<std::string>(header + ".filter")),
                             str2operatorType(pt.get<std::string>(header + ".operator")),
                             pt.get<int>(header + ".order"),
                             pt.get<float>(header + ".turnOnThreshold"),
                             str2thresholdType(pt.get<std::string>(header + ".turnOnThresholdType")),
                             QString::fromStdString(pt.get<std::string>(header + ".turnOnRunPath")),
                             pt.get<float>(header + ".turnOffThreshold"),
                             str2thresholdType(pt.get<std::string>(header + ".turnOffThresholdType")),
                             QString::fromStdString(pt.get<std::string>(header + ".turnOffRunPath")),
                             pt.get<int>(header + ".minRunTimePerDay"),
                             pt.get<int>(header + ".runTimeFulfillmentTimeBorder"),
                             pt.get<bool>(header + ".manualMode"),
                             pt.get<bool>(header + ".ON"));
                    triggers[index]->set(t);
                }
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
