#include "gpio.h"
#include "pigpio.h"


std::shared_ptr<Gpio> Gpio::_instance = nullptr;

QString modeToString(GPIOModes mode)
{
    switch (mode)
    {
    case GPIOModes::INPUT: return "Input";
    case GPIOModes::OUTPUT: return "Output";
    case GPIOModes::ALT0: return "Alt 0";
    case GPIOModes::ALT1: return "Alt 1";
    case GPIOModes::ALT2: return "Alt 2";
    case GPIOModes::ALT3: return "Alt 3";
    case GPIOModes::ALT4: return "Alt 4";
    case GPIOModes::ALT5: return "Alt 5";
    }
}

QString errorToStirng(GPIOErrors err)
{
    switch (err)
    {
    case GPIOErrors::OK: return "Ok";
    case GPIOErrors::BAD_PUD: return "Bad Pull Up Down (PUD) Code";
    case GPIOErrors::BAD_GPIO: return "Bad GPIO Pin Number";
    case GPIOErrors::BAD_MODE: return "Bad GPIO Pin Mode";
    case GPIOErrors::NOT_PERMITTED: return "Mode or PIN not permitted";
    case GPIOErrors::UNKNOWN: return "Unknow Error";
    case GPIOErrors::BAD_LEVEL: return "Bad Level";
    case GPIOErrors::BAD_USER_GPIO: return "Bad user GPIO";
    case GPIOErrors::BAD_DUTYCYCLE: return "Bad duty cycle";
    case GPIOErrors::BAD_DUTYRANGE: return "Bad Duty Range";
    }
}

QString pullUpDownToString(GPIOPullUpDown upDown)
{
    switch (upDown)
    {
    case GPIOPullUpDown::UP: return "Pull Up";
    case GPIOPullUpDown::DOWN: return "Pull Down";
    case GPIOPullUpDown::OFF: return "No pull up or down";
    }
}

QString stateToString(GPIOState state)
{
    switch (state)
    {
    case GPIOState::ON: return "On";
    case GPIOState::OFF: return "Off";
    }
}


Gpio::Gpio()
{
    _init_value = gpioInitialise();
    if (_init_value >= 0)
    {
        _init_value = true;
    }
}

Gpio::~Gpio()
{
    gpioTerminate();
}


unsigned int Gpio::translateMode(GPIOModes mode)
{
    switch (mode)
    {
    case GPIOModes::INPUT: return PI_INPUT;
    case GPIOModes::ALT0: return PI_ALT0;
    case GPIOModes::ALT1: return PI_ALT1;
    case GPIOModes::ALT2: return PI_ALT2;
    case GPIOModes::ALT3: return PI_ALT3;
    case GPIOModes::ALT4: return PI_ALT4;
    case GPIOModes::ALT5: return PI_ALT5;
    case GPIOModes::OUTPUT: return PI_OUTPUT;
    }
    return PI_OUTPUT;
}


GPIOModes Gpio::translateMode(int mode)
{
    switch (mode)
    {
    case PI_INPUT: return GPIOModes::INPUT;
    case PI_ALT0: return GPIOModes::ALT0;
    case PI_ALT1: return GPIOModes::ALT1;
    case PI_ALT2: return GPIOModes::ALT2;
    case PI_ALT3: return GPIOModes::ALT3;
    case PI_ALT4: return GPIOModes::ALT4;
    case PI_ALT5: return GPIOModes::ALT5;
    case PI_OUTPUT: return GPIOModes::OUTPUT;
    }
    return GPIOModes::OUTPUT;
}


int Gpio::translateErrors(GPIOErrors error)
{
    switch (error)
    {
    case GPIOErrors::OK: return 0;
    case GPIOErrors::BAD_GPIO: return PI_BAD_GPIO;
    case GPIOErrors::BAD_MODE: return PI_BAD_MODE;
    case GPIOErrors::NOT_PERMITTED: return PI_NOT_PERMITTED;
    case GPIOErrors::BAD_PUD: return PI_BAD_PUD;
    case GPIOErrors::BAD_LEVEL: return PI_BAD_LEVEL;
    case GPIOErrors::BAD_USER_GPIO: return PI_BAD_USER_GPIO;
    case GPIOErrors::BAD_DUTYCYCLE: return PI_BAD_DUTYCYCLE;
    case GPIOErrors::BAD_DUTYRANGE: return PI_BAD_DUTYRANGE;
    case GPIOErrors::UNKNOWN: return -10000;
    }
    return -10000;
}

GPIOErrors Gpio::translateErrors(int error)
{
    switch (error)
    {
    case 0: return GPIOErrors::OK;
    case PI_BAD_GPIO: return GPIOErrors::BAD_GPIO;
    case PI_BAD_MODE: return GPIOErrors::BAD_MODE;
    case PI_NOT_PERMITTED: return GPIOErrors::NOT_PERMITTED;
    case PI_BAD_PUD: return GPIOErrors::BAD_PUD;
    case PI_BAD_LEVEL: return GPIOErrors::BAD_LEVEL;
    case PI_BAD_USER_GPIO: return GPIOErrors::BAD_USER_GPIO;
    case PI_BAD_DUTYCYCLE: return GPIOErrors::BAD_DUTYCYCLE;
    case PI_BAD_DUTYRANGE: return GPIOErrors::BAD_DUTYRANGE;
    }
    return GPIOErrors::UNKNOWN;
}

unsigned int Gpio::translatePullUpDown(GPIOPullUpDown upDown)
{
    switch (upDown)
    {
    case GPIOPullUpDown::UP: return PI_PUD_UP;
    case GPIOPullUpDown::DOWN: return PI_PUD_DOWN;
    case GPIOPullUpDown::OFF: return PI_PUD_OFF;
    }
    return PI_PUD_UP;
}

unsigned int Gpio::translateState(GPIOState state)
{
    switch (state)
    {
    case GPIOState::ON: return 1;
    case GPIOState::OFF: return 0;
    }
}

GPIOState Gpio::translateState(int level)
{
    switch (level)
    {
    case 0: return GPIOState::OFF;
    case 1: return GPIOState::ON;
    }
    return GPIOState::OFF;
}


void _insternal_callback(int gpio, int level, unsigned int tick, void *userdata)
{
    ((Gpio*)userdata)->_callback(gpio, level, tick);
}


void Gpio::subscribe(unsigned int pin, std::function<void(int,GPIOState, uint32_t)> func)
{
    if (_subscriptions.find(pin) == _subscriptions.end())
    {
        auto err = gpioSetAlertFuncEx(pin, _insternal_callback, this);
        std::vector<std::function<void(int, GPIOState, uint32_t)>> subs { func };
        _subscriptions.insert(std::pair<int, std::vector<std::function<void(int, GPIOState, uint32_t)>>>(pin, subs));
    } else {
        _subscriptions[pin].push_back(func);
    }
}

GPIOErrors Gpio::setMode(unsigned int pin, GPIOModes mode)
{
    return translateErrors(gpioSetMode(pin, translateMode(mode)));
}

GPIOModes Gpio::getMode(unsigned int pin)
{
    return translateMode(gpioGetMode(pin));
}

GPIOErrors Gpio::setPullUpDown(unsigned int pin, GPIOPullUpDown upDown)
{
    return translateErrors(gpioSetPullUpDown(pin, translatePullUpDown(upDown)));
}

GPIOState Gpio::read(unsigned int pin)
{
    return translateState(gpioRead(pin));
}

GPIOErrors Gpio::write(unsigned int pin, GPIOState state)
{
    return translateErrors(gpioWrite(pin, translateState(state)));
}

GPIOErrors Gpio::pwm(unsigned int pin, unsigned int dutycycle)
{
    return translateErrors(gpioPWM(pin, dutycycle));
}

int Gpio::getPWMdutcycle(unsigned int pin)
{
    return gpioGetPWMdutycycle(pin);
}


