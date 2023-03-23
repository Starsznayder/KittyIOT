#ifndef FIGUREDATA_H
#define FIGUREDATA_H

#include <QString>
#include <QVector>
#include <QDataStream>
#include <QColor>
#include <QVector3D>


namespace kitty {
namespace network {
namespace object {

struct FigureHeader
{
    enum class ChartType : quint32
    {
        XY = 0,
        POLAR = 1
    };

    FigureHeader();
    FigureHeader(ChartType _chartType,
                 QString _tabName,
                 QString _chartName,
                 QString _xLabel,
                 QString _yLabel,
                 unsigned _tickCountYorR,
                 unsigned _tickCountXorAngle,
                 float _scaleMinYorR,
                 float _scaleMaxYorR,
                 float _scaleMinXorAngle,
                 float _scaleMaxXorAngle);

    ChartType chartType;
    QString tabName;
    QString chartName;
    QString xLabel;
    QString yLabel;
    unsigned tickCountYorR;
    unsigned tickCountXorAngle;
    float scaleMinYorR;
    float scaleMaxYorR;
    float scaleMinXorAngle;
    float scaleMaxXorAngle;

    friend QDataStream& operator<<(QDataStream& s, const FigureHeader& r);
    friend QDataStream& operator>>(QDataStream& s, FigureHeader& r);

};

struct FigureSeries
{
    enum class LineType : quint32
    {
        SCATTER = 0,
        LINE = 1,
        BAR = 2,
        SPLINE
    };

    FigureSeries();
    FigureSeries(LineType _lineType,
                 Qt::PenStyle _style,
                 float _size,
                 bool _drawDataPoints,
                 bool _drawDataLabels,
                 QString _name,
                 QColor _color,
                 QVector<float> &_dataYorR,
                 QVector<float> &_dataXorAngle,
                 QVector<float> &_dataZ);

    LineType lineType;
    Qt::PenStyle style;
    float size;
    bool drawDataPoints;
    bool drawDataLabels;
    QString name;
    QColor color;
    QVector<float> dataYorR;
    QVector<float> dataXorAngle;
    QVector<float> dataZ;

    static quint32 penStyleToInt(Qt::PenStyle s);
    static Qt::PenStyle intToPenStyle(quint32);

    friend QDataStream& operator<<(QDataStream& s, const FigureSeries& r);
    friend QDataStream& operator>>(QDataStream& s, FigureSeries& r);

};

struct SurfaceSeries
{

    SurfaceSeries();
    SurfaceSeries(QVector<uint8_t> &_data,
                  float _minZ,
                  float _maxZ,
                  float _mean,
                  unsigned sizeX,
                  unsigned sizeY);

    QVector<uint8_t> data;
    float minZ;
    float maxZ;
    float mean;
    unsigned sizeX;
    unsigned sizeY;

    friend QDataStream& operator<<(QDataStream& s, const SurfaceSeries& r);
    friend QDataStream& operator>>(QDataStream& s, SurfaceSeries& r);

};

class FigureData
{
public:
    FigureData();

    friend QDataStream& operator<<(QDataStream& s, const FigureData& r);
    friend QDataStream& operator>>(QDataStream& s, FigureData& r);

    FigureHeader header;
    QVector<FigureSeries> series;
    SurfaceSeries surface;

    static bool isMagic(quint64 magic)
    {
        return magic == Magic;
    }

private:
    static const quint64 Magic = 1224168;
};

}
}
}


#endif // FIGUREDATA_H
