#ifndef POLARWINDOW_H
#define POLARWINDOW_H

#include <QMainWindow>
#include <QtWidgets/QWidget>
#include <QtCore/QVariant>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QPolarChart>

#include "../FigureData.h"
#include "ChartView.h"

class PolarPlot : public ChartView
{
    Q_OBJECT
public:
    explicit PolarPlot(QWidget *parent = nullptr);
    PolarPlot(QSharedPointer<kitty::network::object::FigureData> data,
              QWidget *parent = nullptr);
    ~PolarPlot();

    QSharedPointer<kitty::network::object::FigureData> lastData() const {return lastData_;}
    QString name() const {return name_;}
    bool isActive() const {return isActive_;}
    void setActive(bool a) {isActive_ = a;}

    void legend(bool sw);
    void grid(bool sw);
    void hold(bool sw) {hold_.store(sw);}

public slots:
    void onData(QSharedPointer<kitty::network::object::FigureData>);

private:
    QString name_;
    bool isActive_;
    std::atomic<bool> hold_;
    QSharedPointer<kitty::network::object::FigureData> lastData_;

    QVector<QtCharts::QScatterSeries*> scatters_;
    QVector<QtCharts::QLineSeries*> lines_;
    QVector<QtCharts::QSplineSeries*> splines_;
    QVector<QtCharts::QBarSeries*> bars_;

    void checkDims(const kitty::network::object::FigureHeader& hdr);

    template<typename T>
    void clearSeries(const QVector<T> &series)
    {
        for (int s = 0; s < series.size(); ++s)
        {
            if (series[s]->count() > 0)
            {
                series[s]->clear();
            }
        }
    }

    template<typename T>
    void updateLineSeries(T &series, const kitty::network::object::FigureSeries &data)
    {
        //series.clear();
        series.setName(data.name);
        series.setColor(data.color);
        series.setPointsVisible(data.drawDataPoints);
        series.setPointLabelsVisible(data.drawDataLabels);
        series.pen().setWidth(static_cast<int>(data.size));
        QPen penS= series.pen();
        penS.setStyle(data.style);
        series.setPen(penS);
        series.setVisible(true);

        QVector<QPointF> m_buffer(std::min(data.dataYorR.size(), data.dataXorAngle.size()));

        for (int l = 0; l < std::min(data.dataYorR.size(), data.dataXorAngle.size()); ++l)
        {
            m_buffer[l] = QPointF(data.dataXorAngle[l], data.dataYorR[l]);
        }
        series.replace(m_buffer);
    }
    void mouseMoveEvent(QMouseEvent *event);
};

#endif // POLARWINDOW_H
