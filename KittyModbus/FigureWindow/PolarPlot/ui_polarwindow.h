/********************************************************************************
** Form generated from reading UI file 'polarwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_POLARWINDOW_H
#define UI_POLARWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QPolarChart>
#include "ChartView.h"

QT_BEGIN_NAMESPACE

class Ui_PolarWindow
{
public:

    QtCharts::QPolarChart *chart;
    QtCharts::QValueAxis *angularAxis;
    QtCharts::QValueAxis *radialAxis;
    ChartView *chartView;

    void setupUi(QMainWindow *PolarWindow)
    {
        if (PolarWindow->objectName().isEmpty())
        {
            PolarWindow->setObjectName(QString::fromUtf8("PolarWindow"));
        }
        PolarWindow->resize(800, 600);

        chart = new QtCharts::QPolarChart();
        angularAxis = new QtCharts::QValueAxis();
        angularAxis->setTickCount(9); // First and last ticks are co-located on 0/360 angle.
        angularAxis->setLabelFormat("%.1f");
        angularAxis->setShadesVisible(true);
        angularAxis->setShadesBrush(QBrush(QColor(249, 249, 255)));
        chart->addAxis(angularAxis, QtCharts::QPolarChart::PolarOrientationAngular);

        radialAxis = new QtCharts::QValueAxis();
        radialAxis->setTickCount(9);
        radialAxis->setLabelFormat("%d");
        chart->addAxis(radialAxis, QtCharts::QPolarChart::PolarOrientationRadial);

        radialAxis->setRange(radialMin, radialMax);
        angularAxis->setRange(angularMin, angularMax);

        chartView = new ChartView(angularAxis, radialAxis);
        chartView->setChart(chart);
        chartView->setRenderHint(QPainter::Antialiasing);
        PolarWindow->setCentralWidget(chartView);
        chart->setTitle("Use arrow keys to scroll, +/- to zoom, and space to switch chart type.");
        retranslateUi(PolarWindow);

        QMetaObject::connectSlotsByName(PolarWindow);
    } // setupUi

    void retranslateUi(QMainWindow *PolarWindow)
    {
        PolarWindow->setWindowTitle(QApplication::translate("PolarWindow", "PolarWindow", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PolarWindow: public Ui_PolarWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_POLARWINDOW_H
