
#include <iostream>
#include <chrono>
#include <thread>
#include <iomanip>
#include "Gamepad.h"


int main()
{
    Gamepad gamepad(0);
    bool aPressed = false;
    if (!gamepad.isConnected())
    {
        std::cout << "Controller not connected" << std::endl;
        return -1;
    }

    while (true)
    {
        
        if (gamepad.Update())
        {
            std::cout << std::fixed << std::setprecision(2) << "Left Trigger: " << gamepad.leftTrigger
                << "\r\n" << std::flush;
            
            std::cout << "Left Stick " << "X: " << gamepad.leftStickX
                << ", Y: " << gamepad.leftStickY << "\r\n" << std::flush;
            
            std::cout << "Right Stick " << "X: " << gamepad.rightStickX
                << ", Y: " << gamepad.rightStickY << "\r\n" << std::flush;

            if (gamepad.isButtonPressed(XINPUT_GAMEPAD_A))
            {
                aPressed = true;
            }
            else
            {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              
                aPressed = false;
            }

            if (aPressed == true)
            {
                gamepad.Vibrate(20000, 20000);
            }
            else
            {
                gamepad.resetVibration();
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return 0;
}
