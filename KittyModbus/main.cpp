#include <modbus/modbus.h>
#include <QApplication>
#include <QDebug>
#include "FS.h"
#include "Config.h"
#include "RegDef.h"
#include "Triggers.h"
#include "Process.h"
#include "DataBufer.h"
#include "FigureWindow/FigureWindow.h"

int main(int argc, char *argv[])
{
    QThreadPool::globalInstance()->setMaxThreadCount(20);
    QApplication a(argc, argv);
    Config& config = Config::instance();
    const RegDef& regdef = RegDef::instance();
    Triggers& triggers = Triggers::instance();

    qDebug() << regdef.regInfo.size();

    ModbusReader modbusReader;
    FS filesystem;
    QObject::connect(&modbusReader, SIGNAL(freshData(ModbusData)),
                     &filesystem, SLOT(onData(ModbusData)));
    QVector<QSharedPointer<Process>> processes;
    for (unsigned i = 0; i < triggers.triggers.size(); ++i)
    {
        processes.push_back(QSharedPointer<Process>::create(i));
        QObject::connect(&modbusReader, SIGNAL(freshData(ModbusData)),
                         &*processes.back(), SLOT(onData(ModbusData)));
    }

    DataBufer dataBufer;
    QObject::connect(&modbusReader, SIGNAL(freshData(ModbusData)),
                     &dataBufer, SLOT(onData(ModbusData)));

    kitty::gui::FigureWindow fg(config.figuresWindow.name.get());

    qRegisterMetaType<QSharedPointer<kitty::network::object::FigureData>>("QSharedPointer<kitty::network::object::FigureData>");
    QObject::connect(&dataBufer, SIGNAL(gotData(QSharedPointer<kitty::network::object::FigureData>)),
                     &fg, SLOT(onData(QSharedPointer<kitty::network::object::FigureData>)));

    QObject::connect(&fg, SIGNAL(closeAll()),
                     &modbusReader, SLOT(onDisconnect()));
    modbusReader.onConnect();


    fg.show();
    return a.exec();
}
