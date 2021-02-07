#include <iostream>
#include <pigpio.h>
#include <sstream>
#include <thread>

int main(int argc, char ** argv)
{

    int freq = 0; 

    if (argc < 2)
    {
        std::cout << "USAGE: " << argv[0] << " [freq] -- where freq is an integer " << std::endl;
        return -1;
    }

    if (gpioInitialise() >= 0) {

        std::cout << "Seting gpio 14 to " << argv[1] << std::endl;
        std::stringstream ss;
        ss << argv[1];
        ss >> freq;

        int result = gpioSetPWMfrequency(14, freq);
        std::cout <<  result << std::endl;
        result = gpioSetPWMrange(14, 2000);
        while (true) {
            for (int i = 10; i <=2000; i++)
            {
                result = gpioPWM(14, i);
                std::this_thread::sleep_for(std::chrono::milliseconds(1));    
                
            }
            for (int i = 2000; i >= 10; i--)
            {
                result = gpioPWM(14, i);
                std::this_thread::sleep_for(std::chrono::milliseconds(1));    
                
            }
        }
    } else {
        std::cout << "Error initializing gpio" << std::endl;
    }

    //while (true) std::this_thread::sleep_for(std::chrono::minutes(10));    

    return 0;
}
