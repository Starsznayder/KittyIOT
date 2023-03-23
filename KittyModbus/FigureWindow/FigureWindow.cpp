#include "FigureWindow.h"
#include "ui_figurewindow.h"
#include <QProcess>
#include <QMessageBox>
#include <QMetaType>
#include <QPushButton>

#include "../Config.h"

namespace kitty {
namespace gui{

void FigureWindow::configToActCol()
{
    int colNum = Config::instance().figuresWindow.numOfCols.get();
    colActions[std::max(std::min(4, colNum), 1) - 1]->setEnabled(false);
    colActions[std::max(std::min(4, colNum), 1) - 1]->setChecked(true);
}

void FigureWindow::configToActRow()
{
    int rowNum = Config::instance().figuresWindow.numOfRows.get();
    rowActions[std::max(std::min(4, rowNum), 1) - 1]->setEnabled(false);
    rowActions[std::max(std::min(4, rowNum), 1) - 1]->setChecked(true);
}

FigureWindow::FigureWindow(QString name, QWidget *parent) : QMainWindow(parent),
                                                            name_(name),
                                                            ui(new Ui::FigureWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(name_);
    processingInProgress_.store(false);

    QAction* help = new QAction("Help");
    ui->menubar->addAction(help);
    connect(help, SIGNAL(triggered(bool)), this, SLOT(onHelp(bool)));

    colActions[0] = ui->actionColumns1;
    colActions[1] = ui->actionColumns2;
    colActions[2] = ui->actionColumns3;
    colActions[3] = ui->actionColumns4;
    rowActions[0] = ui->actionRows1;
    rowActions[1] = ui->actionRows2;
    rowActions[2] = ui->actionRows3;
    rowActions[3] = ui->actionRows4;

    configToActCol();
    configToActRow();

    ui->actionGrid->setChecked(Config::instance().figuresWindow.showGrid.get());
    ui->actionLegend->setChecked(Config::instance().figuresWindow.showLegend.get());

    connect(ui->treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*, int)),
            this, SLOT(onTreeItemChanged(QTreeWidgetItem*, int)));

    treeHeader_ = new TreeHeader(Qt::Horizontal, ui->treeWidget);
    ui->treeWidget->setHeader(treeHeader_);
    ui->treeWidget->header()->resizeSection(0 /*column index*/, 400 /*width*/);
    treeHeader_->box->setChecked(true);
    connect(treeHeader_->box, SIGNAL(clicked(bool)), this, SLOT(onHeaderCheckStateChanged(bool)));
}

unsigned FigureWindow::activePlotsNum()
{
    unsigned numOfActive = 0;
    for (int p = 0; p < plots_.size(); ++p)
    {
        if (plots_[p]->isActive())
        {
            ++numOfActive;
        }
    }

    return numOfActive;
}

void FigureWindow::closeEvent(QCloseEvent* event)
{
    qDebug() << "Have a nice day!";
    emit(closeAll());
    this->close();
    QApplication::quit();
}

void FigureWindow::onHelp(bool)
{
    QString help("[space] - Change chart type (Polar and Cartesian)\n"
                 "[LMB] - Hold and move chart with mouse (or use arrow keys)\n"
                 "[Home] - Reset chart settings\n"
                 "[Ctrl] - Enable/Disable tooltips\n\n"
                 "Cartesian:\n"
                 "[Ctrl + LMB] - Hold to select and zoom area\n"
                 "[+] - Zoom in\n"
                 "[-] - Zoom out\n"
                 "[SMB] - Zoom in/out\n\n"
                 "Polar:\n"
                 "[+] - Zoom in (Y/R axis only)\n"
                 "[-] - Zoom out (Y/R axis only)\n"
                 "[SMB] - Zoom (Y/R axis only)\n");

    QMessageBox msgBox;
    msgBox.setText(help);
    msgBox.exec();
}

void FigureWindow::reshapeGrid(unsigned activePlotsCnt, unsigned occupiedColumns)
{
    unsigned rowNum = Config::instance().figuresWindow.numOfRows.get();
    unsigned colNum = Config::instance().figuresWindow.numOfCols.get();
    if (occupiedColumns * rowNum != activePlotsCnt)
    {
        ++occupiedColumns;
    }

    for (unsigned r = 0; r < std::min(rowNum, static_cast<unsigned>(activePlotsCnt)); ++r)
    {
        ui->grid->setRowStretch(r, 1);
        for (unsigned c = 0; c < std::min(colNum, occupiedColumns); ++c)
        {
            ui->grid->setColumnStretch(c, 1);
        }
    }
}

void FigureWindow::reorganizePlots()
{
    unsigned rowNum = Config::instance().figuresWindow.numOfRows.get();
    unsigned colNum = Config::instance().figuresWindow.numOfCols.get();
    unsigned activePlotsCnt = activePlotsNum();
    for (int p = 0; p < plots_.size(); ++p)
    {
        plots_[p]->hide();
    }

    QLayoutItem *wItem;
    while ((wItem = ui->grid->takeAt(0)) != 0)
    {
        if (wItem->widget())
        {
            wItem->widget()->setParent(nullptr);
        }
    }

    ui->horizontalLayout->removeItem(ui->grid);
    delete ui->grid;
    ui->grid = new QGridLayout(ui->layoutWidget);
    ui->horizontalLayout->addLayout(ui->grid);

    unsigned numPlotsToDisplay = std::min(activePlotsCnt, colNum  * rowNum);

    unsigned displayedPlotNum = 0;
    for (int p = 0; p < plots_.size(); ++p)
    {
        if (plots_[p]->isActive() && displayedPlotNum < numPlotsToDisplay)
        {
            unsigned occupiedColumns = static_cast<unsigned>(static_cast<float>(displayedPlotNum) / static_cast<float>(rowNum));
            unsigned row = (displayedPlotNum) - occupiedColumns * rowNum;
            ui->grid->addWidget(plots_[p], row, occupiedColumns, 1, 1);
            plots_[p]->show();
            ++displayedPlotNum;
        }
    }

    reshapeGrid(activePlotsCnt, static_cast<unsigned>(static_cast<float>(numPlotsToDisplay) / static_cast<float>(rowNum)));
}

void FigureWindow::onData(QSharedPointer<kitty::network::object::FigureData> data)
{
    if (!processingInProgress_.load())
    {
        unsigned rowNum = Config::instance().figuresWindow.numOfRows.get();
        unsigned colNum = Config::instance().figuresWindow.numOfCols.get();
        processingInProgress_.store(true);
        bool chartIsPresent = false;
        for (int p = 0; p < plots_.size(); ++p)
        {
            if (plots_[p]->name() == data->header.chartName)
            {
                emit gotData(data);
                //plots_[p]->onData(data);
                chartIsPresent = true;
                break;
            }
        }

        if (!chartIsPresent)
        {
            if (data->surface.data.size() == 0)
            {
                plots_.push_back(new PolarPlot(data));
                connect(this, SIGNAL(gotData(QSharedPointer<kitty::network::object::FigureData>)),
                        plots_.back(),SLOT(onData(QSharedPointer<kitty::network::object::FigureData>)));
                plots_.back()->setActive(true);
                plots_.back()->grid(ui->actionGrid->isChecked());
                plots_.back()->legend(ui->actionLegend->isChecked());
                plots_.back()->hold(ui->actionHold->isChecked());
                unsigned activePlotsCnt = activePlotsNum();

                QTreeWidgetItem* dataForTree = new QTreeWidgetItem();
                dataForTree->setText(0, plots_.back()->name());
                dataForTree->setFlags(dataForTree->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);
                if(treeHeader_->box->checkState())
                {
                    dataForTree->setCheckState(0, Qt::CheckState::Checked);
                }
                else
                {
                    dataForTree->setCheckState(0, Qt::CheckState::Unchecked);
                }
                ui->treeWidget->addTopLevelItem(dataForTree);

                if (activePlotsCnt <= rowNum * colNum)
                {
                    reorganizePlots();
                }
            }
        }
        processingInProgress_.store(false);
    }
}

QVector<QSharedPointer<kitty::network::object::FigureData>> FigureWindow::getLastData()
{
    QVector<QSharedPointer<kitty::network::object::FigureData>> lastData;
    for (int p = 0; p < plots_.size(); ++p)
    {
        lastData.push_back(plots_[p]->lastData());
    }

    return lastData;
}

void FigureWindow::onTreeItemChanged(QTreeWidgetItem* item, int)
{
    QString chartName = item->text(0);

    bool itemSate = item->checkState(0);

    for (int p = 0; p < plots_.size(); ++p)
    {
        if (plots_[p]->name() == chartName)
        {
            plots_[p]->setActive(itemSate);
        }
    }

    if(!itemSate)
    {
        treeHeader_->box->setChecked(false);
    }

    reorganizePlots();
}

void FigureWindow::onHeaderCheckStateChanged(bool)
{
    bool checkBoxState = treeHeader_->box->checkState();
    QTreeWidgetItemIterator it(ui->treeWidget);

    while (*it)
    {
        if(checkBoxState)
            (*it)->setCheckState(0, Qt::CheckState::Checked);
        else
            (*it)->setCheckState(0, Qt::CheckState::Unchecked);
        ++it;
    }

    for (int p = 0; p < plots_.size(); ++p)
    {
        plots_[p]->setActive(checkBoxState);
    }
    reorganizePlots();
}

void FigureWindow::on_actionHold_toggled(bool arg1)
{
    for (int p = 0; p < plots_.size(); ++p)
    {
        plots_[p]->hold(arg1);
    }
}

void FigureWindow::on_actionLegend_toggled(bool arg1)
{
    Config::instance().figuresWindow.showLegend.set(arg1);
    //Config::instance().saveConfig();
    for (int p = 0; p < plots_.size(); ++p)
    {
        plots_[p]->legend(arg1);
    }
}

void FigureWindow::on_actionGrid_toggled(bool arg1)
{
    Config::instance().figuresWindow.showGrid.set(arg1);
    //Config::instance().saveConfig();
    for (int p = 0; p < plots_.size(); ++p)
    {
        plots_[p]->grid(arg1);
    }
}

void FigureWindow::setColRow(QAction* actionArray[4], int idx)
{
    for (unsigned i = 0; i < 4; ++i)
    {
        if (i != idx)
        {
            actionArray[i]->setChecked(false);
            actionArray[i]->setEnabled(true);
        }
    }
    actionArray[idx]->setEnabled(false);

    //Config::instance().saveConfig();
    reorganizePlots();
}

void FigureWindow::on_actionRows1_toggled(bool arg1)
{
    if (arg1)
    {
        Config::instance().figuresWindow.numOfRows.set(1);
        setColRow(rowActions, 0);
    }
}
void FigureWindow::on_actionRows2_toggled(bool arg1)
{
    if (arg1)
    {
        Config::instance().figuresWindow.numOfRows.set(2);
        setColRow(rowActions, 1);
    }
}
void FigureWindow::on_actionRows3_toggled(bool arg1)
{
    if (arg1)
    {
        Config::instance().figuresWindow.numOfRows.set(3);
        setColRow(rowActions, 2);
    }
}
void FigureWindow::on_actionRows4_toggled(bool arg1)
{
    if (arg1)
    {
        Config::instance().figuresWindow.numOfRows.set(4);
        setColRow(rowActions, 3);
    }
}

void FigureWindow::on_actionColumns1_toggled(bool arg1)
{
    if (arg1)
    {
        Config::instance().figuresWindow.numOfCols.set(1);
        setColRow(colActions, 0);
    }
}
void FigureWindow::on_actionColumns2_toggled(bool arg1)
{
    if (arg1)
    {
        Config::instance().figuresWindow.numOfCols.set(2);
        setColRow(colActions, 1);
    }
}
void FigureWindow::on_actionColumns3_toggled(bool arg1)
{
    if (arg1)
    {
        Config::instance().figuresWindow.numOfCols.set(3);
        setColRow(colActions, 2);
    }
}
void FigureWindow::on_actionColumns4_toggled(bool arg1)
{
    if (arg1)
    {
        Config::instance().figuresWindow.numOfCols.set(4);
        setColRow(colActions, 3);
    }
}

}
}
