#ifndef FIGUREGROUP_H
#define FIGUREGROUP_H

#include <QMainWindow>
#include <QVector>
#include <QSharedPointer>
#include <QFuture>

#include "FigureWindow.h"

#include "FigureData.h"

#include <atomic>
#include <mutex>

namespace Ui {
class FigureGroup;
}

namespace kitty{
namespace gui{

class FigureGroup : public QMainWindow
{
    Q_OBJECT
public:
    explicit FigureGroup(QWidget *parent = 0);
    ~FigureGroup();

signals:
    void hidden();
    void data(QSharedPointer<kitty::network::object::FigureData> data);

private slots:
    void onData(QSharedPointer<kitty::network::object::FigureData>);
    void closeTab(int);
    void on_actionOpen_triggered(bool arg1);
    void on_actionSave_As_triggered(bool arg1);

private:
    void closeEvent(QCloseEvent*);
    Ui::FigureGroup *ui;
    QVector<FigureWindow*> windows_;

};


}
}

#endif // GROUPINWINDOW_H
