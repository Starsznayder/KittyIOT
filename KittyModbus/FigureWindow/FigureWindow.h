#ifndef FIGUREWINDOW_H
#define FIGUREWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <mutex>
#include <QTreeWidgetItem>
#include <QApplication>
#include "PolarPlot/PolarPlot.h"
#include "FigureData.h"
#include "TreeHeaderBase.h"

namespace Ui {
    class FigureWindow;
}

namespace kitty{
namespace gui{

class FigureWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit FigureWindow(QString name, QWidget *parent = 0);
    ~FigureWindow(){}

    QString name() const {return name_;}

    QVector<QSharedPointer<kitty::network::object::FigureData>> getLastData();

signals:
    void gotData(QSharedPointer<kitty::network::object::FigureData>);
    void closeAll();

public slots:
    void onData(QSharedPointer<kitty::network::object::FigureData> data);

private slots:
    void onHelp(bool);
    void on_actionRows1_toggled(bool arg1);
    void on_actionRows2_toggled(bool arg1);
    void on_actionRows3_toggled(bool arg1);
    void on_actionRows4_toggled(bool arg1);
    void on_actionColumns1_toggled(bool arg1);
    void on_actionColumns2_toggled(bool arg1);
    void on_actionColumns3_toggled(bool arg1);
    void on_actionColumns4_toggled(bool arg1);
    void on_actionHold_toggled(bool arg1);
    void on_actionLegend_toggled(bool arg1);
    void on_actionGrid_toggled(bool arg1);
    void onTreeItemChanged(QTreeWidgetItem* item, int column);
    void onHeaderCheckStateChanged(bool);

private:
    QString name_;
    Ui::FigureWindow* ui;
    QVector<PolarPlot*> plots_;
    QAction* colActions[4];
    QAction* rowActions[4];

    std::atomic<bool> processingInProgress_;

    TreeHeader* treeHeader_;

    unsigned activePlotsNum();
    void reorganizePlots();
    void reshapeGrid(unsigned activePlotsCnt, unsigned occupiedColumns);
    void configToActCol();
    void configToActRow();

    void setColRow(QAction* actionArray[4], int idx);

    void closeEvent(QCloseEvent* event );
};

}
}

#endif // FIGUREWINDOW_H
