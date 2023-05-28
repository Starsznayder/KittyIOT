#include "ChartsDef.h"
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

void ChartsDef::loadFile(const std::string& configFilepath)
{
    ptree pt;
    std::string prefix("Chart");
    bool gotAll = false;
    int index = 0;

    try{
        ini_parser::read_ini(configFilepath, pt);
        QVector<QSharedPointer<Def>> tmp;
        while (!gotAll)
        {
            try {
                std::string header = prefix + std::to_string(index);
                tmp.push_back(QSharedPointer<Def>::create(Def({QString::fromStdString(pt.get<std::string>(std::string(header + ".name"))),
                                                               pt.get<int>(std::string(header + ".deviceIndex")),
                                                               str2regNumers(pt.get<std::string>(header + ".registers"))})));
            }
            catch (const property_tree::ptree_bad_path&)
            {
                gotAll = true;
            }
            catch (const property_tree::ptree_bad_data&)
            {
                DummyBox::showErrorBox("Chart definition is broken");
            }
            ++index;
        }
        defs.set(tmp);
    }
    catch (const ini_parser::ini_parser_error&)
    {
        DummyBox::showErrorBox("Chart definition is broken");
    }
}
