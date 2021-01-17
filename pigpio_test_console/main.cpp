#include <QDebug>
#include <QCoreApplication>
#include "gpio.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << "Init GPIO Pins";

    auto gpio = Gpio::getGpio();
    gpio->setMode(9, GPIOModes::INPUT);
    auto mode = gpio->getMode(9);

    gpio->subscribe(9, [](int pin, GPIOState state, uint32_t ticks){
        qDebug() << "State change: " << pin << " " << stateToString(state) << " " << ticks;
    });

    qDebug() << modeToString( mode );


    return a.exec();
}
