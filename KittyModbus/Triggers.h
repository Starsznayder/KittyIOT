#ifndef TRIGGERS_H
#define TRIGGERS_H

#include "Config.h"
#include <QString>
#include <QVector>
#include <QSharedPointer>
#include "Filter.h"
#include "MultiregOperator.h"
#include <QFuture>
#include <QtConcurrent>
#include <atomic>

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
        enum class ThresholdType
        {
            LO,
            GT
        };
        QString name;
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

        void update(QString _name,
                    QVector<int> _registers,
                    filters::FilterType filterType,
                    operators::OperatorType operatorType,
                    int _order,
                    float _turnOnThreshold,
                    ThresholdType _turnOnThresholdType,
                    QString _turnOnRunPath,
                    float _turnOffThreshold,
                    ThresholdType _turnOffThresholdType,
                    QString _turnOffRunPath,
                    int _minRunTimePerDay,
                    int _runTimeFulfillmentTimeBorder);
    };

    QVector<QSharedPointer<ConfigValue<Trigger>>> triggers;

private:
    QFuture<void> readerThreadObject;
    std::atomic<bool> finish;

    void loadFile(const std::string& configFilepath);

    Triggers()
    {
        loadFile("ini/triggers.ini");
        finish.store(false);
        readerThreadObject = QtConcurrent::run(this, &Triggers::updateThread);
    };

    void updateThread()
    {
        while (!finish.load())
        {
            QThread::sleep(5);
            loadFile("ini/triggers.ini");
        }
    }
};

#endif // TRIGGERS_H
