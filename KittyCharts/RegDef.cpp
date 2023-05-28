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

RegDef::Type str2type(const std::string& value)
{
    if (value == std::string("float"))
    {
        return RegDef::Type::f;
    }
    return RegDef::Type::i;
}

void RegDef::loadFile(const std::string& configFilepath)
{
    ptree pt;
    const std::string prefix("Reg");
    bool gotAllFiles = false;
    int fileIndex = 0;
    while (!gotAllFiles)
    {
        std::string fullPath = configFilepath + std::to_string(fileIndex) + ".ini";
        try{
            bool gotAll = false;
            int index = 0;
            qDebug() << fullPath.c_str();
            ini_parser::read_ini(fullPath, pt);
            QVector<QSharedPointer<ConfigValue<RegInfo>>> t;
            while (!gotAll)
            {
                try {
                    std::string header = prefix + std::to_string(index);
                    t.push_back(QSharedPointer<ConfigValue<RegInfo>>::create(RegInfo({QString::fromStdString(pt.get<std::string>(header + ".name")),
                                                                                      QString::fromStdString(pt.get<std::string>(header + ".unit")),
                                                                                      pt.get<int>(header + ".regAddr"),
                                                                                      pt.get<int>(header + ".size"),
                                                                                      str2type(pt.get<std::string>(header + ".type"))})));
                }
                catch (const property_tree::ptree_bad_path&)
                {
                    gotAll = true;
                }
                catch (const property_tree::ptree_bad_data&)
                {
                    DummyBox::showErrorBox("Register definition is broken");
                }
                if (t.size() <= 0)
                {
                    DummyBox::showErrorBox("Register definition is broken");
                }
                ++index;
            }
            regInfo.push_back(t);
        }
        catch (const ini_parser::ini_parser_error&)
        {
            gotAllFiles = true;
        }
        ++fileIndex;
    }
    if (regInfo.size() <= 0)
    {
        DummyBox::showErrorBox("No register definition files avaliable");
    }
}
