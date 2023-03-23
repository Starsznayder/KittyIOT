#ifndef CHARTSDEF_H
#define CHARTSDEF_H

#include "Config.h"
#include <QString>
#include <QVector>
#include <QSharedPointer>


class ChartsDef
{
public:
    ChartsDef(const ChartsDef&) = delete;
    ChartsDef& operator=(const ChartsDef&) = delete;

    static ChartsDef& instance()
    {
        static ChartsDef c;
        return c;
    }

    struct Def
    {
        QString name;
        QVector<int> registers;
    };

    QVector<QSharedPointer<ConfigValue<Def>>> defs;

private:
    void loadFile(const std::string& configFilepath);
    ChartsDef() {loadFile("ini/charts.ini");};
};

#endif // TRIGGERS_H
