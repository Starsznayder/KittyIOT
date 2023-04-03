#include "ModbusReader.h"
#include <modbus/modbus.h>
#include <QDebug>
#include <QThread>
#include <QtConcurrent>
#include <chrono>
#include "Config.h"
#include "RegDef.h"

ModbusReader::ModbusReader()
{
    qRegisterMetaType<ModbusData>("ModbusData");
}

void ModbusReader::onDisconnect()
{
    finish.store(true);
    readerThreadObject.waitForFinished();
}
void ModbusReader::onConnect()
{
    finish.store(false);
    readerThreadObject = QtConcurrent::run(this, &ModbusReader::worker);
}

int getNumIters(int target, int coeff)
{
    int t = 0;
    int i = 0;
    while(t < target)
    {
        t += coeff;
        ++i;
    }
    return i;
}

void ModbusReader::worker()
{
    Config& config = Config::instance();
    RegDef& regdef = RegDef::instance();
    while (!finish.load())
    {
        const std::string ifaceName = config.modbusConfig.ifaceName.get();
        const int dataAccessInterval = config.modbusConfig.dataAccessInterval.get();
        const int bitRate = config.modbusConfig.bitRate.get();
        const char parityCheckMethod = static_cast<const char>(config.modbusConfig.parityCheckMethod.get());
        const int dataSizeInSinglePacket = config.modbusConfig.dataSizeInSinglePacket.get();
        const int numStopBits = config.modbusConfig.numStopBits.get();
        const int numRegistersPerQuery = config.modbusConfig.numRegistersPerQuery.get();

        int minRegAddr = 1e6;
        int maxRegAddr = -1;

        for (unsigned i = 0; i < regdef.regInfo.size(); ++i)
        {
            if (regdef.regInfo[i]->get().regAddr > maxRegAddr)
            {
                maxRegAddr = regdef.regInfo[i]->get().regAddr;
            }
            if (regdef.regInfo[i]->get().regAddr < minRegAddr)
            {
                minRegAddr = regdef.regInfo[i]->get().regAddr;
            }
        }

        if (minRegAddr < maxRegAddr)
        {
            maxRegAddr += 2;
            modbus_t *ctx;
            std::vector<uint16_t> tab_reg(numRegistersPerQuery);

            ctx = modbus_new_rtu(ifaceName.c_str(),
                                 bitRate,
                                 parityCheckMethod,
                                 dataSizeInSinglePacket,
                                 numStopBits);
            if (ctx == nullptr)
            {
                _KE("Modbus", "[INFO][Modbus]: Unable to create the libmodbus context");
            }
            else
            {

                if (modbus_connect(ctx) == -1)
                {
                    _KE("Modbus", "[INFO][Modbus]: Connection failed - " << modbus_strerror(errno));
                    modbus_free(ctx);
                }
                else
                {
                    modbus_set_slave(ctx, 1);

                    int numIters = getNumIters(maxRegAddr - minRegAddr, tab_reg.size());
                    QVector<int16_t> buffer(tab_reg.size() * numIters);

                    for (int k = 0; k < numIters; ++k)
                    {
                        int read_val = modbus_read_input_registers(ctx, minRegAddr + k * tab_reg.size(), tab_reg.size(), tab_reg.data());
                        if(read_val==-1)
                        {
                            _KE("Modbus", "[INFO][Modbus]: Something went wrong - " << modbus_strerror(errno));
                        }
                        else
                        {
                            _KI("Modbus", "[INFO][Modbus]: OK");
                            std::copy(tab_reg.begin(), tab_reg.end(), buffer.data() + k * tab_reg.size());
                        }
                    }

                    ModbusData output;
                    const auto p1 = std::chrono::system_clock::now();
                    output.timestamp = std::chrono::duration_cast<std::chrono::microseconds>(p1.time_since_epoch()).count();
                    output.values = QSharedPointer<QVector<float>>::create(regdef.regInfo.size());

                    for (unsigned i = 0; i < regdef.regInfo.size(); ++i)
                    {
                        (*output.values)[i] = reinterpret_cast<float*>(&buffer[regdef.regInfo[i]->get().regAddr - minRegAddr])[0];
                    }

                    emit freshData(output);

                    modbus_close(ctx);
                    modbus_free(ctx);
                }
            }
        }
        QThread::sleep(dataAccessInterval);
    }
}
