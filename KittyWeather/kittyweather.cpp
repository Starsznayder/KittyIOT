
#include "kittyweather.h"
#include "ui_kittyweather.h"


KittyWeather::KittyWeather(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::KittyWeather)
{
    ui->setupUi(this);
}

KittyWeather::~KittyWeather()
{
    delete ui;
}


