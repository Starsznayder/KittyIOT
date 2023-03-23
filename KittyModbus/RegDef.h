#ifndef REGDEF_H
#define REGDEF_H

#include "Config.h"
#include <QString>
#include <QVector>
#include <QSharedPointer>

class RegDef
{
public:

    RegDef(const RegDef&) = delete;
    RegDef& operator=(const RegDef&) = delete;

    static RegDef& instance()
    {
        static RegDef c;
        return c;
    }

    struct RegInfo
    {
        QString name;
        QString unit;
        int regAddr;
    };

    QVector<QSharedPointer<ConfigValue<RegInfo>>> regInfo;

private:
    void loadFile(const std::string& configFilepath);
    RegDef() {loadFile("ini/regDef.ini");};
};

#endif // CONFIG_H
