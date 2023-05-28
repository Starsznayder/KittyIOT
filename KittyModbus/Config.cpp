#include "Config.h"
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/algorithm/string/replace.hpp>

using ptree = boost::property_tree::ptree;
namespace property_tree = boost::property_tree;
namespace ini_parser = boost::property_tree::ini_parser;

Config::ModbusConfig::ParityCheckMethod str2parityCheckMethod(const std::string& value)
{
    if (value == std::string("EVEN"))
    {
        return Config::ModbusConfig::ParityCheckMethod::EVEN;
    }
    else if (value == std::string("ODD"))
    {
        return Config::ModbusConfig::ParityCheckMethod::ODD;
    }
    else
    {
        return Config::ModbusConfig::ParityCheckMethod::NONE;
    }
}

Config::ModbusConfig::RegType str2regType(const std::string& value)
{
    if (value == std::string("STANDARD"))
    {
        return Config::ModbusConfig::RegType::STANDARD;
    }
    return Config::ModbusConfig::RegType::INPUT;
}

void Config::loadFile(const std::string& configFilepath)
{
    ptree pt;

    try {
        ini_parser::read_ini(configFilepath, pt);
        std::string prefix("Modbus");
        bool gotAll = false;
        int index = 0;
        while (!gotAll)
        {

            try {
                std::string header = prefix + std::to_string(index);
                if (modbusConfig.size() <= index)
                {
                    modbusConfig.push_back(QSharedPointer<ConfigValue<ModbusConfig>>::create(ModbusConfig({pt.get<std::string>(header + std::string(".name")),
                                                                                                           pt.get<std::string>(header + std::string(".ifaceName")),
                                                                                                           pt.get<unsigned>(header + std::string(".deviceAddr")),
                                                                                                           pt.get<unsigned>(header + std::string(".dataAccessInterval")),
                                                                                                           pt.get<unsigned>(header + std::string(".bitRate")),
                                                                                                           str2parityCheckMethod(pt.get<std::string>(header + std::string(".parityCheckMethod"))),
                                                                                                           pt.get<unsigned>(header + std::string(".dataSizeInSinglePacket")),
                                                                                                           pt.get<unsigned>(header + std::string(".numStopBits")),
                                                                                                           pt.get<unsigned>(header + std::string(".numRegistersPerQuery")),
                                                                                                           str2regType(pt.get<std::string>(header + std::string(".regType")))})));
                }
                else
                {
                   modbusConfig[index]->set(ModbusConfig({pt.get<std::string>(header + std::string(".name")),
                                                          pt.get<std::string>(header + std::string(".ifaceName")),
                                                          pt.get<unsigned>(header + std::string(".deviceAddr")),
                                                          pt.get<unsigned>(header + std::string(".dataAccessInterval")),
                                                          pt.get<unsigned>(header + std::string(".bitRate")),
                                                          str2parityCheckMethod(pt.get<std::string>(header + std::string(".parityCheckMethod"))),
                                                          pt.get<unsigned>(header + std::string(".dataSizeInSinglePacket")),
                                                          pt.get<unsigned>(header + std::string(".numStopBits")),
                                                          pt.get<unsigned>(header + std::string(".numRegistersPerQuery")),
                                                          str2regType(pt.get<std::string>(header + std::string(".regType")))}));
                }
            }
            catch (const property_tree::ptree_bad_path&)
            {
                gotAll = true;
            }
            catch (const property_tree::ptree_bad_data&)
            {
                DummyBox::showErrorBox("Register definition is broken");
            }
            ++index;
        }

        network.ip(pt.get<std::string>(std::string("DataOutput.ip")));
        network.port(pt.get<unsigned>("DataOutput.port"));

        systemConfig.dataHistoryBufferSize(pt.get<unsigned>(std::string("System.dataHistoryBufferSize")));
        systemConfig.weatherFilePath(pt.get<std::string>(std::string("System.weatherFilePath")));
        systemConfig.dataFilePath(pt.get<std::string>(std::string("System.dataFilePath")));
        systemConfig.logDataToFile(pt.get<bool>(std::string("System.logDataToFile")));

        logs.ip(pt.get<std::string>(std::string("Logs.ip")));
        logs.port(pt.get<unsigned>("Logs.port"));

        systemConfig.ip(pt.get<std::string>(std::string("System.ip")));
    }
    catch (const property_tree::ptree_bad_path&)
    {
        DummyBox::showErrorBox("Configuration is broken");
        throw;
    }
    catch (const property_tree::ptree_bad_data&)
    {
        DummyBox::showErrorBox("Configuration is broken");
        throw;
    }
    catch (const ini_parser::ini_parser_error&)
    {
        DummyBox::showErrorBox("Configuration is broken");
        throw;
    }
}
