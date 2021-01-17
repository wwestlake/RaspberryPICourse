#ifndef GPIO_H
#define GPIO_H

#include <memory>
#include <QString>
#include <map>
#include <vector>
#include <functional>

#include "gpio_global.h"

enum class GPIOErrors {
    OK,
    BAD_GPIO,
    BAD_MODE,
    NOT_PERMITTED,
    BAD_PUD,
    BAD_LEVEL,
    BAD_USER_GPIO,
    BAD_DUTYCYCLE,
    BAD_DUTYRANGE,
    UNKNOWN
};

QString errorToStirng(GPIOErrors err);

enum class GPIOModes {
    INPUT,
    OUTPUT,
    ALT0,
    ALT1,
    ALT2,
    ALT3,
    ALT4,
    ALT5
};

QString modeToString(GPIOModes mode);


enum class GPIOPullUpDown {
    UP,
    DOWN,
    OFF
};

QString pullUpDownToString(GPIOPullUpDown upDown);

enum class GPIOState {
    ON,
    OFF
};

QString stateToString(GPIOState state);


class GPIOSHARED_EXPORT Gpio
{
    static std::shared_ptr<Gpio> _instance;
    bool _initialized = false;
    int _init_value = -1;
    std::map<int, std::vector< std::function<void(int, GPIOState, uint32_t)>>> _subscriptions;
    Gpio();
public:

    ~Gpio();

    static std::shared_ptr<Gpio> getGpio()
    {
        if (_instance == nullptr)
        {
            Gpio* shared = new Gpio();
            _instance = std::shared_ptr<Gpio>(shared);
        }
        return _instance;
    }

    Gpio(const Gpio&) = delete;
    Gpio& operator =(const Gpio&) = delete;

    bool isInitialized() { return _initialized; }
    int getInitValue() { return _initialized; }

    // GPIO control methods
    GPIOErrors setMode(unsigned pin, GPIOModes mode);
    GPIOModes getMode(unsigned pin);
    GPIOErrors setPullUpDown(unsigned int pin, GPIOPullUpDown upDown);

    GPIOState read(unsigned int pin);
    GPIOErrors write(unsigned int pin, GPIOState state);
    GPIOErrors pwm(unsigned int pin, unsigned int dutycycle);
    int getPWMdutcycle(unsigned int pin);

    // typedef void (*gpioAlertFunc_t) (int gpio, int level, uint32_t tick);

    void _callback(int gpio, int level, uint32_t tick)
    {
        if (_subscriptions.find(gpio) != _subscriptions.end())
        {
            for (auto sub : _subscriptions[gpio])
            {
                sub(gpio, translateState(level), tick);
            }
        }
    }

//    static void _insternal_callback(unsigned gpio, int level, uint32_t tick, void *userdata);

    void subscribe(unsigned int pin, std::function<void(int,GPIOState, uint32_t)> func);

private:
    unsigned int translateMode(GPIOModes mode);
    GPIOModes translateMode(int mode);
    int translateErrors(GPIOErrors error);
    GPIOErrors translateErrors(int error);
    unsigned int translatePullUpDown(GPIOPullUpDown upDown);
    unsigned int translateState(GPIOState state);
    GPIOState translateState(int level);
};

#endif // GPIO_H
