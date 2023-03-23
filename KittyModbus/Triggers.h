#ifndef TRIGGERS_H
#define TRIGGERS_H

#include "Config.h"
#include <QString>
#include <QVector>
#include <QSharedPointer>
#include "Filter.h"
#include "MultiregOperator.h"


class Triggers
{
public:

    Triggers(const Triggers&) = delete;
    Triggers& operator=(const Triggers&) = delete;

    static QVector<int> str2regNumers(std::string value);

    static Triggers& instance()
    {
        static Triggers c;
        return c;
    }

    struct Trigger
    {
        enum class Operator
        {
            SUM,
            AVERGE
        };

        enum class Filter
        {
            MEDIAN,
            MEAN,
            MAX,
            MIN
        };

        enum class ThresholdType
        {
            LO,
            GT
        };

        QVector<int> registers;
        QSharedPointer<operators::MultiregOperator> multiregOperator;
        QSharedPointer<filters::Filter> filter;
        int order;
        float turnOnThreshold;
        ThresholdType turnOnThresholdType;
        QString turnOnRunPath;
        float turnOffThreshold;
        ThresholdType turnOffThresholdType;
        QString turnOffRunPath;
        int minRunTimePerDay;
        int runTimeFulfillmentTimeBorder;
    };

    QVector<QSharedPointer<ConfigValue<Trigger>>> triggers;

private:
    void loadFile(const std::string& configFilepath);
    Triggers() {loadFile("ini/triggers.ini");};
};

#endif // TRIGGERS_H
