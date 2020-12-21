#include <iostream>
#include <pigpio.h>
#include <thread>

constexpr int LED_PIN = 24;
constexpr int BUTTON_PIN = 16;


void delay(int ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}


void LED(int state)
{
    gpioWrite(LED_PIN, state);
}


void handle_button(int gpio, int reading, uint32_t tick)
{
    static uint32_t last_tick = 0;
    float duration = (tick - last_tick) / 1000000.0;
    if (duration < 0.05) return;

    LED(reading);

    std::cout << duration << " " << reading << std::endl;
    last_tick = tick;
}

int main(void)
{
    if (gpioInitialise() >= 0)
    {
        gpioSetMode(BUTTON_PIN, PI_INPUT);
        gpioSetPullUpDown(BUTTON_PIN, PI_PUD_DOWN);
        gpioSetAlertFunc(BUTTON_PIN, handle_button);
    }

    while (true) delay(1000);

    return 0;
}



