#ifndef CHARTVIEW_H
#define CHARTVIEW_H

#include <QtCharts/QChartView>
#include <QtCharts/QPolarChart>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLineSeries>
#include <mutex>
#include <atomic>
#include "Callout.h"

static const qreal angularMin = -90;
static const qreal angularMax = 270;

static const qreal radialMin = 0;
static const qreal radialMax = 100;

class ChartView : public QtCharts::QChartView
{
public:
    ChartView(QWidget *parent = 0);

protected:

    void resetAxes();
    void updateTicksNum();

    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void keyReleaseEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void switchChartType();

    QtCharts::QValueAxis* angularOrXAxis_;
    QtCharts::QValueAxis* radialOrYAxis_;

    double maxR_;
    double minR_;
    double maxAngle_;
    double minAngle_;

    unsigned ticksAngle_;
    unsigned ticksRange_;

    bool legendSW_;
    bool gridSW_;
    std::mutex eventProtector_;
    std::atomic<bool> useCustomChartType_;
    std::atomic<bool> enableTooltip_;

    Callout *m_tooltip;

public slots:
    void tooltip(QPointF point, bool state);

private:
    QPointF lastMousePos_;
    QPointF newMousePos_;
    std::atomic<bool> selectAreaMode_;
    QtCharts::QLineSeries* selectionArea_;

    QPointF mouseToChartCoords(QPointF mousePos);
};

#endif
