#ifndef MULTIREGOPERATOR_H
#define MULTIREGOPERATOR_H

#include "ModbusReader.h"

namespace operators{

enum class OperatorType : unsigned
{
    SUM = 0,
    AVERAGE = 1
};

class MultiregOperator
{
public:
    MultiregOperator(const QVector<int> &_registers)
    {
        for (int i = 0; i < _registers.size(); ++i)
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

    virtual bool isCompatible(OperatorType, const QVector<int> &)
    {
        return false;
    }

    bool registersCompatible(const QVector<int> &_registers)
    {
        if(_registers.size() != registers.size())
        {
            return false;
        }

        for (int i = 0; i < _registers.size(); ++i)
        {
            if (registers[i] != _registers[i])
            {
                return false;
            }
        }

        return true;
    }

protected:
    QVector<int> registers;
};

class Sum : public MultiregOperator
{
public:
    Sum(const QVector<int> &_registers) : MultiregOperator(_registers) {}
    virtual float execute(ModbusData values);

    virtual bool isCompatible(OperatorType type, const QVector<int> &_registers)
    {
        if (type == OperatorType::SUM)
        {
            return registersCompatible(_registers);
        }

        return false;
    }
};

class Average : public MultiregOperator
{
public:
    Average(const QVector<int> &_registers) : MultiregOperator(_registers) {}
    virtual float execute(ModbusData values);

    virtual bool isCompatible(OperatorType type, const QVector<int> &_registers)
    {
        if (type == OperatorType::AVERAGE)
        {
            return registersCompatible(_registers);
        }

        return false;
    }
};

}


#endif // MULTIREGOPERATOR_H
