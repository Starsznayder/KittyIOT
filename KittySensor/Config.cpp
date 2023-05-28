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
        std::string prefix("Sensor");
        bool gotAll = false;
        int index = 0;
        while (!gotAll)
        {

            try {
                std::string header = prefix + std::to_string(index);
                if (sensorConfig.size() <= index)
                {
                    sensorConfig.push_back(QSharedPointer<ConfigValue<SensorConfig>>::create(SensorConfig({pt.get<std::string>(header + std::string(".name")),
                                                                                                           pt.get<std::string>(header + std::string(".initScript")),
                                                                                                           pt.get<std::string>(header + std::string(".readScript")),
                                                                                                           pt.get<unsigned>(header + std::string(".dataAccessInterval")),
                                                                                                           pt.get<unsigned>(header + std::string(".deviceIndex"))})));
                }
                else
                {
                    sensorConfig[index]->set(SensorConfig({pt.get<std::string>(header + std::string(".name")),
                                                           pt.get<std::string>(header + std::string(".initScript")),
                                                           pt.get<std::string>(header + std::string(".readScript")),
                                                           pt.get<unsigned>(header + std::string(".dataAccessInterval")),
                                                           pt.get<unsigned>(header + std::string(".deviceIndex"))}));
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
