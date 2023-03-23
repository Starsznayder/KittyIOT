#ifndef MULTIREGOPERATOR_H
#define MULTIREGOPERATOR_H

#include "ModbusReader.h"

namespace operators{

class MultiregOperator
{
public:
    MultiregOperator(const QVector<int> &_registers)
    {
        for (unsigned i = 0; i < _registers.size(); ++i)
        {
            if (_registers[i] >= 0)
            {
                registers.push_back(_registers[i]);
            }
        }
    }
    virtual float execute(ModbusData)
    {
        return 0;
    }
protected:
    QVector<int> registers;
};

class Sum : public MultiregOperator
{
public:
    Sum(const QVector<int> &_registers) : MultiregOperator(_registers) {}
    virtual float execute(ModbusData values);
};

class Average : public MultiregOperator
{
public:
    Average(const QVector<int> &_registers) : MultiregOperator(_registers) {}
    virtual float execute(ModbusData values);
};

}


#endif // MULTIREGOPERATOR_H
