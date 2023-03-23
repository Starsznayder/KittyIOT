#include "ChartView.h"
#include <QtGui/QMouseEvent>
#include <QtCore/QDebug>
#include <QtCharts/QAbstractAxis>
#include <QtCharts/QValueAxis>
#include <QApplication>
#include <QDesktopWidget>

ChartView::ChartView(QWidget *parent) : QChartView(parent)
{
    QPen pen(Qt::gray);
    pen.setWidth(1);

    ticksAngle_ = 9;
    ticksRange_ = 9;

    angularOrXAxis_ = new QtCharts::QValueAxis();
    angularOrXAxis_->setTickCount(ticksAngle_); // First and last ticks are co-located on 0/360 angle.
    //angularOrXAxis_->setLabelFormat("%.1f");
    angularOrXAxis_->setGridLinePen(pen);
    angularOrXAxis_->setTitleBrush(Qt::white);
    angularOrXAxis_->setLabelsBrush(Qt::white);

    radialOrYAxis_ = new QtCharts::QValueAxis();
    radialOrYAxis_->setTickCount(ticksRange_);
    //radialOrYAxis_->setLabelFormat("%d");
    radialOrYAxis_->setGridLinePen(pen);
    radialOrYAxis_->setTitleBrush(Qt::white);
    radialOrYAxis_->setLabelsBrush(Qt::white);

    radialOrYAxis_->setRange(radialMin, radialMax);
    angularOrXAxis_->setRange(angularMin, angularMax);

    setDragMode(QGraphicsView::NoDrag);
    this->setMouseTracking(true);

    QtCharts::QPolarChart* ch = new QtCharts::QPolarChart();
    ch->addAxis(angularOrXAxis_, QtCharts::QPolarChart::PolarOrientationAngular);
    ch->addAxis(radialOrYAxis_, QtCharts::QPolarChart::PolarOrientationRadial);
    this->setChart(ch);

    maxR_ = radialMax;
    minR_ = radialMin;
    maxAngle_ = angularMax;
    minAngle_ = angularMin;
    useCustomChartType_.store(false);
    legendSW_ = false;
    gridSW_ = true;
    chart()->legend()->setAlignment(Qt::AlignLeft);
    chart()->legend()->setVisible(legendSW_);
    radialOrYAxis_->setGridLineVisible(gridSW_);
    angularOrXAxis_->setGridLineVisible(gridSW_);
    chart()->removeAllSeries();
    selectAreaMode_.store(false);
    selectionArea_ = new QtCharts::QLineSeries();
    selectionArea_->setName("User selected");
    selectionArea_->setColor(Qt::gray);
    selectionArea_->pen().setWidth(1);
    QPen penSelection = selectionArea_->pen();
    penSelection.setStyle(Qt::PenStyle::DotLine);
    selectionArea_->setPen(penSelection);
    selectionArea_->setVisible(false);
    chart()->addSeries(selectionArea_);

    enableTooltip_.store(false);
    m_tooltip = nullptr;
}

void ChartView::updateTicksNum()
{
    if(ticksAngle_ > 0 && ticksRange_ > 0)
    {
        angularOrXAxis_->setTickCount(ticksAngle_); // First and last ticks are co-located on 0/360 angle.
        radialOrYAxis_->setTickCount(ticksRange_);
    }
    else
    {
        angularOrXAxis_->applyNiceNumbers();
        radialOrYAxis_->applyNiceNumbers();
    }
}

void ChartView::mousePressEvent(QMouseEvent *event)
{
    eventProtector_.lock();
    if (event->button() == Qt::LeftButton)
    {
        QApplication::setOverrideCursor(QCursor(Qt::SizeAllCursor));
        lastMousePos_ = event->pos();
    }

    event->accept();
    QChartView::mousePressEvent(event);
    eventProtector_.unlock();
}

void ChartView::mouseReleaseEvent(QMouseEvent *event)
{
    eventProtector_.lock();
    if (event->button() == Qt::LeftButton && selectAreaMode_.load())
    {
        QApplication::restoreOverrideCursor();
        QPointF oldCoord = mouseToChartCoords(lastMousePos_);
        QPointF newCoord = mouseToChartCoords(newMousePos_);

        radialOrYAxis_->setRange(std::min(oldCoord.y(),newCoord.y()), std::max(oldCoord.y(),newCoord.y()));
        angularOrXAxis_->setRange(std::min(oldCoord.x(),newCoord.x()), std::max(oldCoord.x(),newCoord.x()));
    }
    else if (event->button() == Qt::LeftButton)
    {
       QApplication::restoreOverrideCursor();
    }

    event->accept();
    QChartView::mouseReleaseEvent(event);
    eventProtector_.unlock();
}


QPointF ChartView::mouseToChartCoords(QPointF mousePos)
{
    QSizeF widSize = this->size();
    QRectF plotSize = chart()->plotArea();
    double offsetX = plotSize.x();
    double offsetY = widSize.height() - plotSize.y() - plotSize.height();

    double axeScaleCoeffX = std::abs(angularOrXAxis_->max() - angularOrXAxis_->min()) / plotSize.width();
    double axeScaleCoeffY = std::abs(radialOrYAxis_->max() - radialOrYAxis_->min()) / plotSize.height();

    double axeExtendedLenX = widSize.width() * axeScaleCoeffX;
    double axeExtendedLenY = widSize.height() * axeScaleCoeffY;

    double y = (((widSize.height() - mousePos.y()) / widSize.height()) *  axeExtendedLenY) + (radialOrYAxis_->min() - offsetY * axeScaleCoeffY);
    double x = (((mousePos.x()) / widSize.width()) * axeExtendedLenX) + (angularOrXAxis_->min() - offsetX * axeScaleCoeffX);

    return QPointF(x,y);
}


void ChartView::mouseMoveEvent(QMouseEvent *event)
{
     eventProtector_.lock();

     if (!selectAreaMode_.load())
     {
         if (event->buttons() == Qt::LeftButton && chart()->chartType() == QtCharts::QChart::ChartTypeCartesian)
         {
             QRectF rec = chart()->plotArea();
             auto dPos = event->pos() - lastMousePos_;
             double deltaX = (dPos.x()/rec.width()) * std::abs(angularOrXAxis_->max() - angularOrXAxis_->min());
             double deltaY = (dPos.y()/rec.height()) *  std::abs(radialOrYAxis_->max() - radialOrYAxis_->min());
             angularOrXAxis_->setMin(angularOrXAxis_->min() - deltaX);
             angularOrXAxis_->setMax(angularOrXAxis_->max() - deltaX);
             radialOrYAxis_->setMin(radialOrYAxis_->min() + deltaY);
             radialOrYAxis_->setMax(radialOrYAxis_->max() + deltaY);

             lastMousePos_ = event->pos();
             event->accept();
         }
         else if (event->buttons() == Qt::LeftButton)
         {
             QRectF rec = chart()->plotArea();
             auto dPos = event->pos() - lastMousePos_;
             double deltaY = ((dPos.y())/rec.height()) * std::abs(radialOrYAxis_->max() - radialOrYAxis_->min());
             radialOrYAxis_->setMin(radialOrYAxis_->min() - deltaY);
             radialOrYAxis_->setMax(radialOrYAxis_->max() - deltaY);

             lastMousePos_ = event->pos();
             event->accept();
         }
     }
     else
     {
         if (event->buttons() == Qt::LeftButton && chart()->chartType() == QtCharts::QChart::ChartTypeCartesian)
         {
             newMousePos_= event->pos();
             QPointF oldCoord = mouseToChartCoords(lastMousePos_);
             QPointF newCoord = mouseToChartCoords(newMousePos_);

             selectionArea_->clear();

             selectionArea_->append(oldCoord.x(),oldCoord.y());
             selectionArea_->append(newCoord.x(),oldCoord.y());
             selectionArea_->append(newCoord.x(),newCoord.y());
             selectionArea_->append(oldCoord.x(),newCoord.y());
             selectionArea_->append(oldCoord.x(),oldCoord.y());

             event->accept();
         }
     }

     QChartView::mouseMoveEvent(event);
     eventProtector_.unlock();
}

void ChartView::keyPressEvent(QKeyEvent *event)
{
    eventProtector_.lock();
    switch (event->key()) {
    case Qt::Key_Plus:
    {
        if (chart()->chartType() == QtCharts::QChart::ChartTypeCartesian)
        {
            double yScaleLen = radialOrYAxis_->max() - radialOrYAxis_->min();
            double xScaleLen = angularOrXAxis_->max() - angularOrXAxis_->min();
            double maxX = angularOrXAxis_->max();
            double minX = angularOrXAxis_->min();
            double maxY = radialOrYAxis_->max();
            double minY = radialOrYAxis_->min();

            radialOrYAxis_->setMax(maxY - 0.01 * yScaleLen);
            radialOrYAxis_->setMin(minY + 0.01 * yScaleLen);
            angularOrXAxis_->setMax(maxX - 0.01 * xScaleLen);
            angularOrXAxis_->setMin(minX + 0.01 * xScaleLen);
        }
        else
        {
            if(std::abs(radialOrYAxis_->max()) > std::abs(radialOrYAxis_->min()))
            {
                radialOrYAxis_->setMax(radialOrYAxis_->max() - 0.05 * radialOrYAxis_->max());
            }
            else
            {
                radialOrYAxis_->setMin(radialOrYAxis_->min() - 0.05 * std::abs(radialOrYAxis_->min()));
            }
        }
        break;
    }
    case Qt::Key_Minus:
    {
        if (chart()->chartType() == QtCharts::QChart::ChartTypeCartesian)
        {
            double yScaleLen = radialOrYAxis_->max() - radialOrYAxis_->min();
            double xScaleLen = angularOrXAxis_->max() - angularOrXAxis_->min();
            double maxX = angularOrXAxis_->max();
            double minX = angularOrXAxis_->min();
            double maxY = radialOrYAxis_->max();
            double minY = radialOrYAxis_->min();

            radialOrYAxis_->setMax(maxY + 0.01 * yScaleLen);
            radialOrYAxis_->setMin(minY - 0.01 * yScaleLen);
            angularOrXAxis_->setMax(maxX + 0.01 * xScaleLen);
            angularOrXAxis_->setMin(minX - 0.01 * xScaleLen);
        }
        else
        {
            if(std::abs(radialOrYAxis_->max()) > std::abs(radialOrYAxis_->min()))
            {
                radialOrYAxis_->setMax(radialOrYAxis_->max() + 0.05 * radialOrYAxis_->max());
            }
            else
            {
                radialOrYAxis_->setMin(radialOrYAxis_->min() + 0.05 * std::abs(radialOrYAxis_->min()));
            }
        }
        break;
    }
    case Qt::Key_Left:
        angularOrXAxis_->setMin(angularOrXAxis_->min() - 1.0);
        angularOrXAxis_->setMax(angularOrXAxis_->max() - 1.0);
        break;
    case Qt::Key_Right:
        angularOrXAxis_->setMin(angularOrXAxis_->min() + 1.0);
        angularOrXAxis_->setMax(angularOrXAxis_->max() + 1.0);
        break;
    case Qt::Key_Up:
        radialOrYAxis_->setMin(radialOrYAxis_->min() + 1.0);
        radialOrYAxis_->setMax(radialOrYAxis_->max() + 1.0);
        break;
    case Qt::Key_Down:
        radialOrYAxis_->setMin(radialOrYAxis_->min() - 1.0);
        radialOrYAxis_->setMax(radialOrYAxis_->max() - 1.0);
        break;
    case Qt::Key_Space:
    {
        switchChartType();
        useCustomChartType_.store(true);
        break;
    }
    case Qt::Key_Home:
    {
        resetAxes();
        useCustomChartType_.store(false);
        break;
    }
    case Qt::Key_Control:
    {
        selectAreaMode_.store(true);
        selectionArea_->setVisible(true);
        if(!enableTooltip_.load())
        {
            enableTooltip_.store(true);
        }
        else
        {
            enableTooltip_.store(false);
            if(m_tooltip)
            {
                m_tooltip->hide();
            }
        }
        break;
    }
    default:
        QGraphicsView::keyPressEvent(event);
        break;
    }

    eventProtector_.unlock();
}

void ChartView::keyReleaseEvent(QKeyEvent *event)
{
    eventProtector_.lock();
    if (event->key() == Qt::Key_Control)
    {
        selectAreaMode_.store(false);
        selectionArea_->clear();
        selectionArea_->setVisible(false);
    }
    eventProtector_.unlock();
}

void ChartView::resetAxes()
{
    radialOrYAxis_->setRange(minR_, maxR_);
    angularOrXAxis_->setRange(minAngle_, maxAngle_);
}

void ChartView::wheelEvent(QWheelEvent *event)
{
    eventProtector_.lock();

    if (chart()->chartType() == QtCharts::QChart::ChartTypeCartesian)
    {
        double yScaleLen = radialOrYAxis_->max() - radialOrYAxis_->min();
        double xScaleLen = angularOrXAxis_->max() - angularOrXAxis_->min();
        double maxX = angularOrXAxis_->max();
        double minX = angularOrXAxis_->min();
        double maxY = radialOrYAxis_->max();
        double minY = radialOrYAxis_->min();

        if(event->angleDelta().y() < 0)
        {
             radialOrYAxis_->setMax(maxY + 0.01 * yScaleLen);
             radialOrYAxis_->setMin(minY - 0.01 * yScaleLen);
             angularOrXAxis_->setMax(maxX + 0.01 * xScaleLen);
             angularOrXAxis_->setMin(minX - 0.01 * xScaleLen);
        }
        else
        {
            radialOrYAxis_->setMax(maxY - 0.01 * yScaleLen);
            radialOrYAxis_->setMin(minY + 0.01 * yScaleLen);
            angularOrXAxis_->setMax(maxX - 0.01 * xScaleLen);
            angularOrXAxis_->setMin(minX + 0.01 * xScaleLen);
        }
    }
    else
    {
        if(std::abs(radialOrYAxis_->max()) > std::abs(radialOrYAxis_->min()))
        {
            if(event->angleDelta().y() < 0)
            {
                radialOrYAxis_->setMax(radialOrYAxis_->max() + 0.05 * radialOrYAxis_->max());
            }
            else
            {
                radialOrYAxis_->setMax(radialOrYAxis_->max() - 0.05 * radialOrYAxis_->max());
            }
        }
        else
        {
            if(event->angleDelta().y() < 0)
            {
                radialOrYAxis_->setMin(radialOrYAxis_->min() + 0.05 * std::abs(radialOrYAxis_->min()));
            }
            else
            {
                radialOrYAxis_->setMin(radialOrYAxis_->min() - 0.05 * std::abs(radialOrYAxis_->min()));
            }
        }
    }

    event->accept();
    QChartView::wheelEvent(event);
    eventProtector_.unlock();
}

void ChartView::switchChartType()
{
    QtCharts::QChart *newChart;
    QtCharts::QChart *oldChart = chart();

    if (oldChart->chartType() == QtCharts::QChart::ChartTypeCartesian)
        newChart = new QtCharts::QPolarChart();
    else
        newChart = new QtCharts::QChart();

    // Move series and axes from old chart to new one
    const QList<QtCharts::QAbstractSeries *> seriesList = oldChart->series();
    const QList<QtCharts::QAbstractAxis *> axisList = oldChart->axes();
    QList<QPair<qreal, qreal> > axisRanges;

    for (QtCharts::QAbstractAxis *axis : axisList)
    {
        QtCharts::QValueAxis *valueAxis = static_cast<QtCharts::QValueAxis *>(axis);
        axisRanges.append(QPair<qreal, qreal>(valueAxis->min(), valueAxis->max()));
    }

    for (QtCharts::QAbstractSeries *series : seriesList)
        oldChart->removeSeries(series);

    for (QtCharts::QAbstractAxis *axis : axisList)
    {
        oldChart->removeAxis(axis);
        newChart->addAxis(axis, axis->alignment());
    }

    for (QtCharts::QAbstractSeries *series : seriesList)
    {
        newChart->addSeries(series);
        for (QtCharts::QAbstractAxis *axis : axisList)
            series->attachAxis(axis);
    }

    int count = 0;
    for (QtCharts::QAbstractAxis *axis : axisList)
    {
        axis->setRange(axisRanges[count].first, axisRanges[count].second);
        count++;
    }

    newChart->setTitle(oldChart->title());
    setChart(newChart);
    chart()->setBackgroundBrush(Qt::black);
    chart()->setTitleBrush(Qt::white);
    chart()->legend()->setVisible(legendSW_);
    chart()->legend()->setAlignment(Qt::AlignLeft);
    chart()->legend()->setLabelColor(Qt::white);
    radialOrYAxis_->setGridLineVisible(gridSW_);
    angularOrXAxis_->setGridLineVisible(gridSW_);

    if(m_tooltip)
    {
        m_tooltip->updateParent(newChart);
    }

    delete oldChart;
}

void ChartView::tooltip(QPointF point, bool state)
{
    bool enabled = enableTooltip_.load();
    if (!m_tooltip && enabled)
    {
        m_tooltip = new Callout(chart());
    }

    if (state && enabled)
    {
        m_tooltip->setText(QString("X: %1 \nY: %2 ").arg(point.x()).arg(point.y()));
        m_tooltip->setAnchor(point);
        m_tooltip->setZValue(1000);
        m_tooltip->updateGeometry();
        m_tooltip->show();
    }
    else if (enabled)
    {
        m_tooltip->hide();
    }
}

