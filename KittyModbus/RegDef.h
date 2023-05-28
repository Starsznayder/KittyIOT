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

    enum class Type : unsigned
    {
        f = 0,
        i = 1
    };

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
        int size;
        Type type;
    };

    QVector<QVector<QSharedPointer<ConfigValue<RegInfo>>>> regInfo;

private:
    void loadFile(const std::string& configFilepath);
    RegDef() {loadFile("/kitty/IOT/KittyModbus/ini/regDef");};
};

#endif // CONFIG_H
