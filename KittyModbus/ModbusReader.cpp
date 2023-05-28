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
    qRegisterMetaType<SensorsData>("SensorsData");
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
    return std::max(i, 1);
}

void ModbusReader::onStart(unsigned idx, unsigned addr)
{
    proctor.lock();
    commands.push(Command({idx, addr, static_cast<uint16_t>(1)}));
    proctor.unlock();
}

void ModbusReader::onStop(unsigned idx, unsigned addr)
{
    proctor.lock();
    commands.push(Command({idx, addr, static_cast<uint16_t>(0)}));
    proctor.unlock();
}

modbus_t* reconectModbus(modbus_t *ctx,
                    const Config::ModbusConfig& mc,
                    const char parityCheckMethod,
                    std::atomic<bool>* finishFlag)
{
    _KE(mc.name.c_str(), "RECONNECTING!");
    if (ctx != nullptr)
    {
        modbus_flush(ctx);
        modbus_close(ctx);
        modbus_free(ctx);
        ctx = nullptr;
    }
    while (ctx == nullptr && !finishFlag->load())
    {
        ctx = modbus_new_rtu(mc.ifaceName.c_str(),
                             mc.bitRate,
                             parityCheckMethod,
                             mc.dataSizeInSinglePacket,
                             mc.numStopBits);
        if (ctx == nullptr)
        {
            _KE(mc.name.c_str(), "[INFO][Modbus]: Unable to create the libmodbus context");
        }
        else
        {
            if (modbus_connect(ctx) == -1)
            {
                _KE(mc.name.c_str(), "[INFO][Modbus]: Connection failed - " << modbus_strerror(errno));
                modbus_free(ctx);
                ctx = nullptr;
            }
            else
            {
                uint32_t old_response_to_sec;
                uint32_t old_response_to_usec;
                modbus_get_response_timeout(ctx, &old_response_to_sec, &old_response_to_usec);
                _KI(mc.name.c_str(), "[INFO][ResponseTimeout]: " << old_response_to_sec << "." << old_response_to_usec);
                modbus_set_response_timeout(ctx, 1, 0);
                modbus_get_indication_timeout(ctx, &old_response_to_sec, &old_response_to_usec);
                _KI(mc.name.c_str(), "[INFO][IndicationTimeout]: " << old_response_to_sec << "." << old_response_to_usec);
                modbus_set_indication_timeout(ctx, 1, 0);
            }
        }
        QThread::sleep(mc.dataAccessInterval);
    }
    return ctx;
}

void ModbusReader::worker()
{
    Config& config = Config::instance();
    RegDef& regdef = RegDef::instance();
    modbus_t *ctx = nullptr;

    while (!finish.load())
    {        
        for (int ic = 0; ic < config.modbusConfig.size(); ++ic)
        {
            const Config::ModbusConfig mc = config.modbusConfig[ic]->get();
            const char parityCheckMethod = static_cast<const char>(mc.parityCheckMethod);

            if (ctx == nullptr)
            {
                ctx = reconectModbus(ctx, mc, parityCheckMethod, &finish);
            }

            if (ctx != nullptr)
            {
                modbus_set_slave(ctx, mc.deviceAddr);
                proctor.lock();
                if (commands.size() > 0)
                {
                    if (commands.front().idx == ic)
                    {
                        int status = -1;
                        while (status < 0 && !finish.load())
                        {
                            status = modbus_write_register(ctx, commands.front().addr, commands.front().value);
                            if(status == -1)
                            {
                                _KE(mc.name.c_str(), "[INFO][Write]: Something went wrong - " << modbus_strerror(errno));
                                ctx = reconectModbus(ctx, mc, parityCheckMethod, &finish);
                                modbus_set_slave(ctx, mc.deviceAddr);
                            }
                            else
                            {
                                _KI(mc.name.c_str(), "[INFO][Write]: OK");
                            }
                        }
                        commands.pop();
                    }
                }

                proctor.unlock();
            }

            int minRegAddr = 1e6;
            int maxRegAddr = -1;
            for (int i = 0; i < regdef.regInfo[ic].size(); ++i)
            {
                maxRegAddr = std::max(maxRegAddr, regdef.regInfo[ic][i]->get().regAddr);
                minRegAddr = std::min(minRegAddr, regdef.regInfo[ic][i]->get().regAddr);
            }
            if (minRegAddr <= maxRegAddr)
            {
                maxRegAddr += 2;
                std::vector<uint16_t> tab_reg(std::min(static_cast<int>(mc.numRegistersPerQuery), maxRegAddr - minRegAddr));

                if (ctx == nullptr)
                {
                    ctx = reconectModbus(ctx, mc, parityCheckMethod, &finish);
                }

                if (ctx != nullptr)
                {
                    modbus_set_slave(ctx, mc.deviceAddr);
                    int numIters = getNumIters(maxRegAddr - minRegAddr, tab_reg.size());
                    QVector<int16_t> buffer(tab_reg.size() * numIters);
                    bool broken = false;
                    for (int k = 0; k < numIters; ++k)
                    {
                        int status = -1;
                        while (status < 0 && !finish.load())
                        {
                            if (mc.regType == Config::ModbusConfig::RegType::INPUT)
                            {
                                status = modbus_read_input_registers(ctx, minRegAddr + k * tab_reg.size(), tab_reg.size(), tab_reg.data());
                            }
                            else
                            {
                                status = modbus_read_registers(ctx, minRegAddr + k * tab_reg.size(), tab_reg.size(), tab_reg.data());
                            }
                            if(status == -1)
                            {
                                _KE(mc.name.c_str(), "[INFO][Modbus]: Something went wrong - " << modbus_strerror(errno));
                                broken = true;
                                ctx = reconectModbus(ctx, mc, parityCheckMethod, &finish);
                                modbus_set_slave(ctx, mc.deviceAddr);
                            }
                            else
                            {
                                _KI(mc.name.c_str(), "[INFO][Modbus]: OK");
                                std::copy(tab_reg.begin(), tab_reg.end(), buffer.data() + k * tab_reg.size());
                            }
                        }
                    }

                    if (!broken)
                    {
                        SensorsData output;
                        const auto p1 = std::chrono::system_clock::now();
                        output.timestamp = std::chrono::duration_cast<std::chrono::microseconds>(p1.time_since_epoch()).count();
                        output.values = QSharedPointer<QVector<float>>::create(regdef.regInfo[ic].size());
                        output.devIndex = ic;
                        for (int i = 0; i < regdef.regInfo[ic].size(); ++i)
                        {
                            uint8_t ttb[4] = {0, 0, 0, 0};
                            uint8_t* ptr = reinterpret_cast<uint8_t*>(&buffer[regdef.regInfo[ic][i]->get().regAddr - minRegAddr]);
                            for (unsigned b = 0; b < 2*regdef.regInfo[ic][i]->get().size; ++b)
                            {
                                ttb[b] = ptr[b];
                            }
                            if (regdef.regInfo[ic][i]->get().type == RegDef::Type::f)
                            {
                                (*output.values)[i] = reinterpret_cast<float*>(ttb)[0];
                            }
                            else if (regdef.regInfo[ic][i]->get().type == RegDef::Type::i)
                            {
                                (*output.values)[i] = reinterpret_cast<int*>(ttb)[0];
                            }
                        }

                        emit freshData(output);
                        QSharedPointer<kitty::network::object::SensorsMulticastMSG> mmmsg =
                                QSharedPointer<kitty::network::object::SensorsMulticastMSG>::create(*output.values, output.timestamp, ic);
                        emit sensorsMulticastMSG(mmmsg);
                    }
                }
            }
            QThread::sleep(mc.dataAccessInterval);
        }
    }
    if(ctx != nullptr)
    {
        modbus_flush(ctx);
        modbus_close(ctx);
        modbus_free(ctx);
    }
}
