#include "SunTrigger.h"

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

SunTrigger::Trigger::ThresholdType SunTrigger::str2thresholdType(const std::string& value)
{
    if (value == std::string("RISE"))
    {
        return SunTrigger::Trigger::ThresholdType::RISE;
    }
    else if (value == std::string("FALL"))
    {
        return SunTrigger::Trigger::ThresholdType::FALL;
    }
    else if (value == std::string("NIGHT"))
    {
        return SunTrigger::Trigger::ThresholdType::NIGHT;
    }
    else
    {
        return SunTrigger::Trigger::ThresholdType::DAY;
    }
}

SunTrigger::Trigger::Mode SunTrigger::str2mode(const std::string& value)
{
    if (value == std::string("MODBUS"))
    {
        return SunTrigger::Trigger::Mode::MODBUS;
    }
    else
    {
        return SunTrigger::Trigger::Mode::SCRIPT;
    }
}

void SunTrigger::Trigger::update(QString _name,
                                 Mode _mode,
                                 unsigned _onOffRegisterAddr,
                                 unsigned _modbusDevID,
                                 QString _actionPath,
                                 ThresholdType _thresholdType,
                                 int _offset,
                                 int _delay,
                                 bool _manualMode,
                                 bool _oN)
{
    name = _name;
    mode = _mode;
    onOffRegisterAddr = _onOffRegisterAddr;
    modbusDevID = _modbusDevID;
    actionPath = _actionPath;
    thresholdType = _thresholdType;
    offset = _offset;
    delay = _delay;
    manualMode = _manualMode;
    oN = _oN;
}

void SunTrigger::loadFile(const std::string& configFilepath)
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
                                                                                             QString::fromStdString(pt.get<std::string>(header + ".actionPath")),
                                                                                             str2thresholdType(pt.get<std::string>(header + ".thresholdType")),
                                                                                             pt.get<int>(header + ".offset"),
                                                                                             pt.get<int>(header + ".delay"),
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
                             QString::fromStdString(pt.get<std::string>(header + ".actionPath")),
                             str2thresholdType(pt.get<std::string>(header + ".thresholdType")),
                             pt.get<int>(header + ".offset"),
                             pt.get<int>(header + ".delay"),
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
                DummyBox::showErrorBox("SunTriger definition is broken");
            }
            ++index;
        }
    }
    catch (const ini_parser::ini_parser_error&)
    {
        DummyBox::showErrorBox("SunTriger definition is broken");
    }
}
