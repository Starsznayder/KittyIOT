#include "FigureGroup.h"
#include "ui_figuregroup.h"

#include <QMessageBox>
#include <QDebug>
#include <QtConcurrent>
#include <QFileDialog>
#include <QFile>
#include "../Config.h"

namespace kitty{
namespace gui{

FigureGroup::FigureGroup(QWidget *parent) : QMainWindow(parent),
                                            ui(new Ui::FigureGroup)
{
    ui->setupUi(this);
    QMainWindow::setWindowIcon(QIcon("img/diag.ico"));
    setWindowTitle(Config::instance().figuresWindow.name.get());
    ui->ChartTabs->setTabsClosable(true);
    //resizeDocks({ui->dockWidget}, {0}, Qt::Horizontal);
    connect(ui->ChartTabs, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    connect(this, SIGNAL(data(QSharedPointer<kitty::network::object::FigureData>)), this, SLOT(onData(QSharedPointer<kitty::network::object::FigureData>)));
}


void FigureGroup::closeTab(int index)
{
    if (index == -1)
    {
        return;
    }

    QWidget* tabItem = ui->ChartTabs->widget(index);
    FigureWindow* widow = reinterpret_cast<FigureWindow*>(tabItem);
    for (int i = 0; i < windows_.size(); ++i)
    {
        if (windows_[i]->name() == widow->name())
        {
            windows_.remove(i);
            break;
        }
    }
    ui->ChartTabs->removeTab(index);

    delete(tabItem);
    tabItem = nullptr;
}

void FigureGroup::onData(QSharedPointer<kitty::network::object::FigureData> data)
{
    bool isInContainer = false;
    for (int i = 0; i < windows_.size(); ++i)
    {
        if (windows_[i]->name() == data->header.tabName)
        {
            windows_[i]->onData(data);
            isInContainer = true;
            break;
        }
    }

    if (!isInContainer)
    {
        windows_.push_back(new FigureWindow(data->header.tabName));
        //windows_.back()->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        ui->ChartTabs->addTab(windows_.back(), data->header.tabName);
        //windows_.back()->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        windows_.back()->onData(data);
    }
}

void FigureGroup::on_actionOpen_triggered(bool)
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open Session"), "", tr("DNCharts (*.dnc);; All Files (*)"));
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly))
    {
        QDataStream in(&file);
        in.setVersion(QDataStream::Qt_5_0);
        while(!in.atEnd())
        {
            QSharedPointer<kitty::network::object::FigureData> d = QSharedPointer<kitty::network::object::FigureData>::create();
            quint64 Magic;
            in >> Magic;
            in >> *d;
            emit data(d);
        }
        file.close();
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Unable to create file");
        msgBox.exec();
    }
}

void FigureGroup::on_actionSave_As_triggered(bool)
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save Session"), "", tr("DNCharts (*.dnc);; All Files (*)"));

    QVector<QVector<QSharedPointer<kitty::network::object::FigureData>>> lastData;

    for (int i = 0; i < windows_.size(); ++i)
    {
        lastData.push_back(windows_[i]->getLastData());
    }

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly))
    {

        QDataStream out(&file);
        out.setVersion(QDataStream::Qt_5_0);
        for (int w = 0; w < lastData.size(); ++w)
        {
            for (int p = 0; p < lastData[w].size(); ++p)
            {
                out << (*lastData[w][p]);
            }
        }
        file.close();
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Unable to create file");
        msgBox.exec();
    }

}

FigureGroup::~FigureGroup()
{

}

void FigureGroup::closeEvent(QCloseEvent*)
{
    emit hidden();
}

}
}
