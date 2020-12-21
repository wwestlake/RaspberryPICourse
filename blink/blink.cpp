#include <iostream>
#include <pigpio.h>
#include <thread>

constexpr int LED = 24;

void steady_blink()
{
    while (true) {
        gpioWrite(LED, 1);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        gpioWrite(LED, 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}



int main(void)
{
    if (gpioInitialise() >= 0)
    {
        gpioSetMode(LED, PI_OUTPUT);
        steady_blink();
    }

    return 0;
}