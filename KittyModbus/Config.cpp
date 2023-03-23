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

void Config::loadFile(const std::string& configFilepath)
{
    ptree pt;

    try {
        ini_parser::read_ini(configFilepath, pt);

        modbusConfig.ifaceName(pt.get<std::string>(std::string("Modbus.ifaceName")));
        modbusConfig.dataAccessInterval(pt.get<unsigned>(std::string("Modbus.dataAccessInterval")));
        modbusConfig.bitRate(pt.get<unsigned>(std::string("Modbus.bitRate")));
        modbusConfig.parityCheckMethod(str2parityCheckMethod(pt.get<std::string>(std::string("Modbus.parityCheckMethod"))));
        modbusConfig.dataSizeInSinglePacket(pt.get<unsigned>(std::string("Modbus.dataSizeInSinglePacket")));
        modbusConfig.numStopBits(pt.get<unsigned>(std::string("Modbus.numStopBits")));
        modbusConfig.numRegistersPerQuery(pt.get<unsigned>(std::string("Modbus.numRegistersPerQuery")));
        systemConfig.dataHistoryBufferSize(pt.get<unsigned>(std::string("System.dataHistoryBufferSize")));
        figuresWindow.name(QString::fromStdString(pt.get<std::string>(std::string("FiguresWindow.name"))));
        figuresWindow.showGrid(pt.get<bool>("FiguresWindow.showGrid"));
        figuresWindow.showLegend(pt.get<bool>("FiguresWindow.showLegend"));
        figuresWindow.numOfCols(pt.get<unsigned>("FiguresWindow.numOfCols"));
        figuresWindow.numOfRows(pt.get<unsigned>("FiguresWindow.numOfRows"));
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
