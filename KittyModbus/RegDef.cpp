#include "RegDef.h"
#include <QMessageBox>
#include <QDebug>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/algorithm/string/replace.hpp>

using ptree = boost::property_tree::ptree;
namespace property_tree = boost::property_tree;
namespace ini_parser = boost::property_tree::ini_parser;

void RegDef::loadFile(const std::string& configFilepath)
{
    ptree pt;
    std::string prefix("Reg");
    bool gotAll = false;
    int index = 0;

    try{
        qDebug() << configFilepath.c_str();
        ini_parser::read_ini(configFilepath, pt);
        while (!gotAll)
        {
            try {
                std::string header = prefix + std::to_string(index);
                regInfo.push_back(QSharedPointer<ConfigValue<RegInfo>>::create(RegInfo({QString::fromStdString(pt.get<std::string>(header + ".name")),
                                                                                        QString::fromStdString(pt.get<std::string>(header + ".unit")),
                                                                                        pt.get<int>(header + ".regAddr")})));
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
    }
    catch (const ini_parser::ini_parser_error&)
    {
        DummyBox::showErrorBox("Register definition is broken");
    }
}
