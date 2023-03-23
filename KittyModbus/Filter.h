#ifndef FILTER_H
#define FILTER_H

#include <QVector>

namespace filters{

class Filter
{
public:
    Filter(int _order) : order(std::max(_order, 1)) {}

    virtual float execute(const float value)
    {
        return value;
    }
protected:
    int order;
    QVector<float> circBuffer;
};

class Median : public Filter
{
public:
    Median(int _order) : Filter(_order) {}
    virtual float execute(const float value);
};

class Mean : public Filter
{
public:
    Mean(int _order) : Filter(_order)  {}
    virtual float execute(const float value);
};

class Max : public Filter
{
public:
    Max(int _order) : Filter(_order)  {}
    virtual float execute(const float value);
};

class Min : public Filter
{
public:
    Min(int _order) : Filter(_order)  {}
    virtual float execute(const float value);
};

}

#endif // FILTER_H
