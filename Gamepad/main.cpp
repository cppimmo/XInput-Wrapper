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
            system("cls");
            std::cout << std::fixed << std::setprecision(2) << "Left Trigger: " << gamepad.leftTrigger <<
                ", Right Trigger: " << gamepad.rightTrigger << "\n" <<

            "Left Stick " << "X: " << gamepad.leftStickX
            << ", Y: " << gamepad.leftStickY << "\n" <<

            "Right Stick " << "X: " << gamepad.rightStickX
            << ", Y: " << gamepad.rightStickY << std::endl;

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

            if (gamepad.isButtonPressed(XINPUT_GAMEPAD_BACK))
                break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return 0;
}
