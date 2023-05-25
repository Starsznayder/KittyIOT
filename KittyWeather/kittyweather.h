
#ifndef KITTYWEATHER_H
#define KITTYWEATHER_H

#include <QMainWindow>



QT_BEGIN_NAMESPACE
namespace Ui { class KittyWeather; }
QT_END_NAMESPACE

class KittyWeather : public QMainWindow

{
    Q_OBJECT

public:
    KittyWeather(QWidget *parent = nullptr);
    ~KittyWeather();

private:
    Ui::KittyWeather *ui;
};

#endif // KITTYWEATHER_H
