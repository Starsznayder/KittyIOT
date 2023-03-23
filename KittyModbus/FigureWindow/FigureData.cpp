#include "FigureData.h"

namespace kitty {
namespace network {
namespace object {


QDataStream& operator<<(QDataStream& s, const FigureHeader& fh)
{
    s << static_cast<quint32>(fh.chartType)
      << fh.tabName 
      << fh.chartName 
      << fh.xLabel 
      << fh.yLabel 
      << fh.tickCountYorR
      << fh.tickCountXorAngle
      << fh.scaleMinYorR
      << fh.scaleMaxYorR
      << fh.scaleMinXorAngle
      << fh.scaleMaxXorAngle;
    
    return s;
}

QDataStream& operator>>(QDataStream& s, FigureHeader& fh)
{
    quint32 chartType;
    s >> chartType;
    s >> fh.tabName;
    s >> fh.chartName;
    s >> fh.xLabel;
    s >> fh.yLabel;
    s >> fh.tickCountYorR;
    s >> fh.tickCountXorAngle;
    s >> fh.scaleMinYorR;
    s >> fh.scaleMaxYorR;
    s >> fh.scaleMinXorAngle;
    s >> fh.scaleMaxXorAngle;
    
    fh.chartType = static_cast<FigureHeader::ChartType>(chartType);
    
    return s;
}

FigureHeader::FigureHeader() {}
FigureHeader::FigureHeader(ChartType _chartType,
                           QString _tabName,
                           QString _chartName,
                           QString _xLabel,
                           QString _yLabel,
                           unsigned _tickCountYorR,
                           unsigned _tickCountXorAngle,
                           float _scaleMinYorR,
                           float _scaleMaxYorR,
                           float _scaleMinXorAngle,
                           float _scaleMaxXorAngle) : chartType(_chartType),
                                                      tabName(_tabName),
                                                      chartName(_chartName),
                                                      xLabel(_xLabel),
                                                      yLabel(_yLabel),
                                                      tickCountYorR(_tickCountYorR),
                                                      tickCountXorAngle(_tickCountXorAngle),
                                                      scaleMinYorR(_scaleMinYorR),
                                                      scaleMaxYorR(_scaleMaxYorR),
                                                      scaleMinXorAngle(_scaleMinXorAngle),
                                                      scaleMaxXorAngle(_scaleMaxXorAngle) {}

QDataStream& operator<<(QDataStream& s, const SurfaceSeries& r)
{
    s << r.data
      << r.minZ
      << r.maxZ
      << r.mean
      << r.sizeX
      << r.sizeY;

    return s;
}

QDataStream& operator>>(QDataStream& s, SurfaceSeries& r)
{
    s >> r.data;
    s >> r.minZ;
    s >> r.maxZ;
    s >> r.mean;
    s >> r.sizeX;
    s >> r.sizeY;

    return s;
}

SurfaceSeries::SurfaceSeries() {};
SurfaceSeries::SurfaceSeries(QVector<uint8_t> &_data,
                             float _minZ,
                             float _maxZ,
                             float _mean,
                             unsigned _sizeX,
                             unsigned _sizeY) : data(_data),
                                                minZ(_minZ),
                                                maxZ(_maxZ),
                                                mean(_mean),
                                                sizeX(_sizeX),
                                                sizeY(_sizeY){}

quint32 FigureSeries::penStyleToInt(Qt::PenStyle s)
{
    if (s == Qt::PenStyle::NoPen)
    {
        return 0;
    }
    else if (s == Qt::PenStyle::SolidLine)
    {
        return 1;
    }
    else if (s == Qt::PenStyle::DashLine)
    {
        return 2;
    }
    else if (s == Qt::PenStyle::DotLine)
    {
        return 3;
    }
    else
    {
        return 4;
    }
}

Qt::PenStyle FigureSeries::intToPenStyle(quint32 s)
{
    if (s == 0)
    {
        return Qt::PenStyle::NoPen;
    }
    else if (s == 1)
    {
        return Qt::PenStyle::SolidLine;
    }
    else if (s == 2)
    {
        return Qt::PenStyle::DashLine;
    }
    else if (s == 3)
    {
        return Qt::PenStyle::DotLine;
    }
    else
    {
        return Qt::PenStyle::DashDotDotLine;
    }
}

QDataStream& operator<<(QDataStream& s, const FigureSeries& fs)
{
    s << static_cast<quint32>(fs.lineType)
      << fs.penStyleToInt(fs.style)
      << fs.size
      << fs.drawDataPoints
      << fs.drawDataLabels
      << fs.name
      << fs.color
      << fs.dataYorR
      << fs.dataXorAngle
      << fs.dataZ;
    
    return s;
}

QDataStream& operator>>(QDataStream& s, FigureSeries& fs)
{
    quint32 lineType;
    quint32 style;
    s >> lineType;
    s >> style;
    s >> fs.size;
    s >> fs.drawDataPoints;
    s >> fs.drawDataLabels;
    s >> fs.name;
    s >> fs.color;
    s >> fs.dataYorR;
    s >> fs.dataXorAngle;
    s >> fs.dataZ;
    
    fs.style = fs.intToPenStyle(style);
    fs.lineType = static_cast<FigureSeries::LineType>(lineType);
            
    return s;
}

FigureSeries::FigureSeries() {}
FigureSeries::FigureSeries(LineType _lineType,
                           Qt::PenStyle _style,
                           float _size,
                           bool _drawDataPoints,
                           bool _drawDataLabels,
                           QString _name,
                           QColor _color,
                           QVector<float> &_dataYorR,
                           QVector<float> &_dataXorAngle,
                           QVector<float> &_dataZ) : lineType(_lineType),
                                                            style(_style),
                                                            size(_size),
                                                            drawDataPoints(_drawDataPoints),
                                                            drawDataLabels(_drawDataLabels),
                                                            name(_name),
                                                            color(_color),
                                                            dataYorR(_dataYorR),
                                                            dataXorAngle(_dataXorAngle),
                                                            dataZ(_dataZ){}


QDataStream& operator<<(QDataStream& s, const FigureData& fd)
{
    s << fd.Magic
      << fd.header
      << fd.series
      << fd.surface;

    return s;
}

QDataStream& operator>>(QDataStream& s, FigureData& fd)
{
    s >> fd.header;
    s >> fd.series;
    s >> fd.surface;

    return s;
}

FigureData::FigureData(){}

}
}
}
