#include "Config.h"
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/algorithm/string/replace.hpp>

using ptree = boost::property_tree::ptree;
namespace property_tree = boost::property_tree;
namespace ini_parser = boost::property_tree::ini_parser;

void Config::loadFile(const std::string& configFilepath)
{
    ptree pt;

    try {
        ini_parser::read_ini(configFilepath, pt);
        network.ip(pt.get<std::string>(std::string("DataInput.ip")));
        network.port(pt.get<unsigned>("DataInput.port"));
        systemConfig.weatherFilePath(pt.get<std::string>(std::string("System.weatherFilePath")));
        systemConfig.ip(pt.get<std::string>(std::string("System.ip")));
        logs.ip(pt.get<std::string>(std::string("Logs.ip")));
        logs.port(pt.get<unsigned>("Logs.port"));
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
