#include "DataBufer.h"
#include "Config.h"
#include "RegDef.h"
#include "ChartsDef.h"
#include <QColor>
#include <QDebug>
#include <numeric>

DataBufer::DataBufer(std::string ip, unsigned port, unsigned timeout) :net_(ip, port, timeout, false) {}

uint64_t timestamp;
QSharedPointer<QVector<float>> values;


void DataBufer::onData(SensorsData values)
{
    Config& config = Config::instance();
    RegDef& regdef = RegDef::instance();

    std::string namePrefix("Data");
    namePrefix += std::to_string(values.devIndex);

    if (values.devIndex >= regdef.regInfo.size())
    {
        return;
    }

    for (int i = 0; i < std::min(values.values->size(), regdef.regInfo[values.devIndex].size()); ++i)
    {
        _KI(namePrefix.c_str(), "[INFO][" << regdef.regInfo[values.devIndex][i]->get().name.toStdString() << " ("
            << regdef.regInfo[values.devIndex][i]->get().unit.toStdString() << ")" << "]: " << (*values.values)[i])
    }
    _KI(namePrefix.c_str(), "[INFO][timestamp]: " << values.timestamp);

    QVector<QSharedPointer<ChartsDef::Def>> defs = ChartsDef::instance().defs.get();
    const  QColor palete[6] = {QColor(Qt::blue), QColor(Qt::yellow), QColor(Qt::green), QColor(Qt::red), QColor(Qt::cyan), QColor(Qt::magenta)};

    const int devIndex = values.devIndex;

    if (data.size() <= devIndex)
    {
         data.resize(devIndex + 1);
         timestamps.resize(devIndex + 1);
    }

    if(data[devIndex].size() < values.values->size())
    {
        data[devIndex].resize(values.values->size());
    }
    for (int i = 0; i < values.values->size(); ++i)
    {
        data[devIndex][i].push_back((*values.values)[i]);
    }
    timestamps[devIndex].push_back(values.timestamp);

    int numElemsToRemove = 0;
    while (timestamps[devIndex][numElemsToRemove] <
           timestamps[devIndex].back() - static_cast<uint64_t>(config.systemConfig.dataHistoryBufferSize.get()) * static_cast<uint64_t>(1000000))
    {
        ++numElemsToRemove;
    }
    if (numElemsToRemove > 0)
    {
        timestamps[devIndex].remove(0,numElemsToRemove);
        for (int i = 0; i < data[devIndex].size(); ++i)
        {
            data[devIndex][i].remove(0,numElemsToRemove);
        }
    }

    QVector<float> xVec(timestamps[devIndex].size());
    for (int k = 0; k < xVec.size(); ++k)
    {
        xVec[k] = -(static_cast<double>(timestamps[devIndex].back() - timestamps[devIndex][k]) / 1e6) / 3600.0;
    }

    for (int i = 0; i < defs.size(); ++i)
    {
        if (defs[i]->deviceIndex == values.devIndex)
        {
            QVector<int> tmp = defs[i]->registers;
            QVector<int> regsToTake;
            for (int t = 0; t < std::min(tmp.size(), 6); ++t)
            {
                if(tmp[t] < regdef.regInfo[values.devIndex].size() && tmp[t] >= 0)
                {
                    regsToTake.push_back(tmp[t]);
                }
            }

            if(regsToTake.size() > 0 && timestamps[devIndex].size() > 1)
            {
                float minV = std::numeric_limits<float>::infinity();
                float maxV = -std::numeric_limits<float>::infinity();

                for (int r = 0; r < regsToTake.size(); ++r)
                {
                    if (regsToTake[r] > 0 && regsToTake[r] < data[devIndex].size())
                    {
                        for (int k = 0; k < data[devIndex][regsToTake[r]].size(); ++k)
                        {
                            minV = std::min(minV, data[devIndex][regsToTake[r]][k]);
                            maxV = std::max(maxV, data[devIndex][regsToTake[r]][k]);
                        }
                    }
                }
                QSharedPointer<kitty::network::object::FigureData> fd = QSharedPointer<kitty::network::object::FigureData>::create();
                fd->header = kitty::network::object::FigureHeader(kitty::network::object::FigureHeader::ChartType::XY,
                                                                  QString("Kitty"),
                                                                  defs[i]->name,
                                                                  "Timestamp [h]",
                                                                  regdef.regInfo[values.devIndex][regsToTake[0]]->get().unit,
                                                                  10,
                                                                  12,
                                                                  minV - std::abs(0.1 * (maxV + minV)) / 2,
                                                                  maxV + std::abs(0.1 * (maxV + minV)) / 2,
                                                                  xVec[0],
                                                                  xVec.back());
                fd->series.resize(regsToTake.size());
                for (int s = 0; s < regsToTake.size(); ++s)
                {
                    fd->series[s] = kitty::network::object::FigureSeries(kitty::network::object::FigureSeries::LineType::LINE,
                                                                         Qt::PenStyle::SolidLine,
                                                                         1,
                                                                         false,
                                                                         false,
                                                                         regdef.regInfo[values.devIndex][regsToTake[s]]->get().name,
                                                                         palete[s],
                                                                         data[devIndex][regsToTake[s]],
                                                                         xVec,
                                                                         xVec);
                }
                net_.send(fd);
            }
        }
    }
}

