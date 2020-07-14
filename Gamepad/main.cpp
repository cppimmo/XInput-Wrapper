
#include <iostream>
#include <chrono>
#include <thread>
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
           std::cout << "Left Trigger: " << gamepad.leftTrigger
             << "Left Stick " << "X: " << gamepad.leftStickX
                << ", Y: " << gamepad.leftStickY
            << "Right Stick " << "X: " << gamepad.rightStickX
             << ", Y: " << gamepad.rightStickY << "\r";
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
