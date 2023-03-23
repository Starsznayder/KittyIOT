#include "PolarPlot.h"
#include <QDebug>

PolarPlot::PolarPlot(QWidget *parent) : ChartView(parent)
{

    this->setRenderHint(QPainter::Antialiasing);
    chart()->setTitle("Use arrow keys to scroll, +/- to zoom, and space to switch chart type.");

    QMetaObject::connectSlotsByName(this);
}

PolarPlot::PolarPlot(QSharedPointer<kitty::network::object::FigureData> data,
                     QWidget *parent) : ChartView(parent)
{
    name_ = data->header.chartName;
    this->setRenderHint(QPainter::Antialiasing);
    chart()->setTitle(name_);

    QMetaObject::connectSlotsByName(this);

    for (int s = 0; s < 50; ++s)
    {
        scatters_.push_back(new QtCharts::QScatterSeries);
        scatters_.back()->setVisible(false);
        scatters_.back()->setPointLabelsColor(Qt::white);
        scatters_.back()->setBorderColor(Qt::black);
        chart()->addSeries(scatters_.back());
        connect(scatters_.back(), &QScatterSeries::hovered, this, &PolarPlot::tooltip);
        //scatters_.back()->setUseOpenGL();

        splines_.push_back(new QtCharts::QSplineSeries);
        splines_.back()->setVisible(false);
        splines_.back()->setPointLabelsColor(Qt::white);
        chart()->addSeries(splines_.back());
        connect(splines_.back(), &QLineSeries::hovered, this, &PolarPlot::tooltip);
        //splines_.back()->setUseOpenGL();

        lines_.push_back(new QtCharts::QLineSeries);
        lines_.back()->setVisible(false);
        lines_.back()->setPointLabelsColor(Qt::white);
        chart()->addSeries(lines_.back());
        connect(lines_.back(), &QLineSeries::hovered, this, &PolarPlot::tooltip);
        //lines_.back()->setUseOpenGL();
    }

    radialOrYAxis_->setTitleText(data->header.yLabel);
    angularOrXAxis_->setTitleText(data->header.xLabel);

    onData(data);
    switchChartType();
    switchChartType();
}

void PolarPlot::legend(bool sw)
{
    legendSW_ = sw;
    chart()->legend()->setVisible(sw);
}
void PolarPlot::grid(bool sw)
{
    gridSW_ = sw;
    radialOrYAxis_->setGridLineVisible(gridSW_);
    angularOrXAxis_->setGridLineVisible(gridSW_);
}

void PolarPlot::checkDims(const kitty::network::object::FigureHeader& hdr)
{
    bool axisUpdateNeeded = false;
    if (maxR_ != hdr.scaleMaxYorR)
    {
        maxR_ = hdr.scaleMaxYorR;
        axisUpdateNeeded = true;
    }
    if (minR_ != hdr.scaleMinYorR)
    {
        minR_ = hdr.scaleMinYorR;
        axisUpdateNeeded = true;
    }

    if (maxAngle_ != hdr.scaleMaxXorAngle)
    {
        maxAngle_ = hdr.scaleMaxXorAngle;
        axisUpdateNeeded = true;
    }
    if (minAngle_ != hdr.scaleMinXorAngle)
    {
        minAngle_ = hdr.scaleMinXorAngle;
        axisUpdateNeeded = true;
    }

    if (axisUpdateNeeded)
    {
        resetAxes();
    }

    axisUpdateNeeded = false;

    if (ticksRange_ != hdr.tickCountYorR)
    {
        ticksRange_ =  hdr.tickCountYorR;
        axisUpdateNeeded = true;
    }
    if (ticksAngle_ != hdr.tickCountXorAngle)
    {
        ticksAngle_ = hdr.tickCountXorAngle;
        axisUpdateNeeded = true;
    }

    if (axisUpdateNeeded)
    {
        updateTicksNum();
    }
}

void PolarPlot::onData(QSharedPointer<kitty::network::object::FigureData> data)
{
    if (data->header.chartName != name_ || (hold_.load() && lastData_))
    {
        return;
    }

    eventProtector_.lock();


    if(data->header.chartType == kitty::network::object::FigureHeader::ChartType::POLAR &&
            chart()->chartType() == QtCharts::QChart::ChartTypeCartesian && !useCustomChartType_.load())
    {
        switchChartType();
    }
    else if(data->header.chartType == kitty::network::object::FigureHeader::ChartType::XY &&
            chart()->chartType() == QtCharts::QChart::ChartTypePolar && !useCustomChartType_.load())
    {
        switchChartType();
    }

    clearSeries<QtCharts::QScatterSeries*>(scatters_);
    clearSeries<QtCharts::QLineSeries*>(lines_);
    clearSeries<QtCharts::QSplineSeries*>(splines_);

    checkDims(data->header);

    int scatterSeriesCounter = 0;
    int lineSeriesCounter = 0;
    int splineSeriesCounter = 0;

    for (int i = 0; i < data->series.size(); ++i)
    {
        if (data->series[i].lineType == kitty::network::object::FigureSeries::LineType::LINE)
        {
            if (data->series[i].dataYorR.size() == data->series[i].dataXorAngle.size() && data->series[i].dataYorR.size() > 0)
            {
                if (lines_.size() <= lineSeriesCounter)
                {
                    lines_.push_back(new QtCharts::QLineSeries());
                    chart()->addSeries(lines_.back());
                }
                updateLineSeries(*lines_[lineSeriesCounter], data->series[i]);
                ++lineSeriesCounter;
            }
        }
        else if (data->series[i].lineType == kitty::network::object::FigureSeries::LineType::SPLINE)
        {
            if (data->series[i].dataYorR.size() == data->series[i].dataXorAngle.size() && data->series[i].dataYorR.size() > 0)
            {
                if (splines_.size() <= splineSeriesCounter)
                {
                    splines_.push_back(new QtCharts::QSplineSeries());
                    chart()->addSeries(splines_.back());
                }

                //splines_[splineSeriesCounter]->clear();
                updateLineSeries(*splines_[splineSeriesCounter], data->series[i]);
                ++splineSeriesCounter;
            }
        }
        else if (data->series[i].lineType == kitty::network::object::FigureSeries::LineType::SCATTER)
        {
            if (data->series[i].dataYorR.size() == data->series[i].dataXorAngle.size() && data->series[i].dataYorR.size() > 0)
            {
                if (scatters_.size() <= scatterSeriesCounter)
                {
                    scatters_.push_back(new QtCharts::QScatterSeries());
                    chart()->addSeries(scatters_.back());
                }
                scatters_[scatterSeriesCounter]->clear();
                scatters_[scatterSeriesCounter]->setName(data->series[i].name);
                scatters_[scatterSeriesCounter]->setColor(data->series[i].color);
                scatters_[scatterSeriesCounter]->setMarkerSize(data->series[i].size);
                scatters_[scatterSeriesCounter]->setPointLabelsVisible(data->series[i].drawDataLabels);

                if (data->series[i].style == Qt::PenStyle::DotLine)
                {
                    scatters_[scatterSeriesCounter]->setMarkerShape(QtCharts::QScatterSeries::MarkerShapeCircle);
                }
                else
                {
                    scatters_[scatterSeriesCounter]->setMarkerShape(QtCharts::QScatterSeries::MarkerShapeRectangle);
                }
                scatters_[scatterSeriesCounter]->setVisible(true);

                for (int l = 0; l < std::min(data->series[i].dataYorR.size(), data->series[i].dataXorAngle.size()); ++l)
                {
                    scatters_[scatterSeriesCounter]->append(data->series[i].dataXorAngle[l], data->series[i].dataYorR[l]);
                }
                ++scatterSeriesCounter;
            }
        }
    }

    for (int i = scatterSeriesCounter; i < scatters_.size(); ++i)
    {
        scatters_[i]->setVisible(false);
    }

    for (int i = splineSeriesCounter; i < splines_.size(); ++i)
    {
        splines_[i]->setVisible(false);
    }

    for (int i = lineSeriesCounter; i < lines_.size(); ++i)
    {
        lines_[i]->setVisible(false);
    }

    lastData_ = data;
    eventProtector_.unlock();
}

void PolarPlot::mouseMoveEvent(QMouseEvent *event)
{
    QString xlabel = angularOrXAxis_->titleText();
    QString ylabel = radialOrYAxis_->titleText();
    angularOrXAxis_->setTitleText(xlabel.mid(0, xlabel.indexOf(QString("\n"))) + QString("\n(%1)").arg(chart()->mapToValue(event->pos()).x()));
    radialOrYAxis_->setTitleText(ylabel.mid(0, ylabel.indexOf(QString("\n"))) + QString("\n(%1)").arg(chart()->mapToValue(event->pos()).y()));
    ChartView::mouseMoveEvent(event);
}

PolarPlot::~PolarPlot()
{

}
