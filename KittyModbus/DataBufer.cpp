#include "DataBufer.h"
#include "Config.h"
#include "RegDef.h"
#include "ChartsDef.h"
#include <QColor>
#include <QDebug>
#include <numeric>

DataBufer::DataBufer()
{

}

void DataBufer::onData(ModbusData values)
{
    Config& config = Config::instance();
    RegDef& regdef = RegDef::instance();
    QVector<QSharedPointer<ChartsDef::Def>> defs = ChartsDef::instance().defs.get();
    const  QColor palete[6] = {QColor(Qt::blue), QColor(Qt::yellow), QColor(Qt::green), QColor(Qt::red), QColor(Qt::cyan), QColor(Qt::magenta)};

    if(data.size() < values.values->size())
    {
        data.resize(values.values->size());
    }

    for (unsigned i = 0; i < values.values->size(); ++i)
    {
        data[i].push_back((*values.values)[i]);
    }
    timestamps.push_back(values.timestamp);

    int numElemsToRemove = 0;
    while (timestamps[numElemsToRemove] <
           timestamps.back() - static_cast<uint64_t>(config.systemConfig.dataHistoryBufferSize.get()) * static_cast<uint64_t>(1000000))
    {
        ++numElemsToRemove;
    }

    if (numElemsToRemove > 0)
    {
        timestamps.remove(0,numElemsToRemove);
        for (unsigned i = 0; i < data.size(); ++i)
        {
            data[i].remove(0,numElemsToRemove);
        }
    }

    QVector<float> xVec(timestamps.size());
    for (unsigned k = 0; k < xVec.size(); ++k)
    {
        xVec[k] = -(static_cast<double>(timestamps.back() - timestamps[k]) / 1e6) / 3600.0;
    }

    for (unsigned i = 0; i < defs.size(); ++i)
    {
        QVector<int> tmp = defs[i]->registers;
        QVector<int> regsToTake;
        for (unsigned t = 0; t < std::min(tmp.size(), 6); ++t)
        {
            if(tmp[t] < regdef.regInfo.size() && tmp[t] >= 0)
            {
                regsToTake.push_back(tmp[t]);
            }
        }

        if(regsToTake.size() > 0 && timestamps.size() > 1)
        {
            float minV = std::numeric_limits<float>::infinity();
            float maxV = -std::numeric_limits<float>::infinity();

            for (unsigned r = 0; r < regsToTake.size(); ++r)
            {
                if (regsToTake[r] > 0 && regsToTake[r] < data.size())
                {
                    for (unsigned k = 0; k < data[regsToTake[r]].size(); ++k)
                    {
                        minV = std::min(minV, data[regsToTake[r]][k]);
                        maxV = std::max(maxV, data[regsToTake[r]][k]);
                    }
                }
            }
            QSharedPointer<kitty::network::object::FigureData> fd = QSharedPointer<kitty::network::object::FigureData>::create();
            fd->header = kitty::network::object::FigureHeader(kitty::network::object::FigureHeader::ChartType::XY,
                                                              QString("Kitty"),
                                                              defs[i]->name,
                                                              "Timestamp [h]",
                                                              regdef.regInfo[regsToTake[0]]->get().unit,
                                                              10,
                                                              12,
                                                              minV - std::abs(0.1 * (maxV + minV)) / 2,
                                                              maxV + std::abs(0.1 * (maxV + minV)) / 2,
                                                              xVec[0],
                                                              xVec.back());
            fd->series.resize(regsToTake.size());
            for (unsigned s = 0; s < regsToTake.size(); ++s)
            {
                fd->series[s] = kitty::network::object::FigureSeries(kitty::network::object::FigureSeries::LineType::LINE,
                                                                     Qt::PenStyle::SolidLine,
                                                                     1,
                                                                     false,
                                                                     false,
                                                                     regdef.regInfo[regsToTake[s]]->get().name,
                                                                     palete[s],
                                                                     data[regsToTake[s]],
                                                                     xVec,
                                                                     xVec);
            }
            emit(gotData(fd));
        }
    }
}

