#include <iostream>
#include <chrono>
#include <thread>
#include <iomanip>
#include <cstdlib>
#include "Gamepad.h"

static auto Clear() -> void;

auto main(int argc, char** argv) -> int
{
	using Button = Gamepad::Button;
	Gamepad gamepad(1u);
	std::cout << sizeof(gamepad) << '\n';
	bool aPressed = false;
	if (!gamepad.IsConnected())
	{
		std::cout << "Controller not connected\n";
		std::cout.flush();
		std::cin.get();
		return -1;
	}

	while (true)
	{
		if (gamepad.Update())
		{
			::Clear();
			std::cout << std::fixed << std::setprecision(2)
				<< "Left Trigger: " << gamepad.LeftTrigger()
				<< ", Right Trigger: " << gamepad.RightTrigger() << '\n'
				<< "Left Stick " << "X: " << gamepad.LeftStick().x
				<< ", Y: " << gamepad.LeftStick().y << '\n'
				<< "Right Stick " << "X: " << gamepad.RightStick().x
				<< ", Y: " << gamepad.RightStick().y << std::endl;

			if (gamepad.IsButtonPressed(Button::A))
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
				gamepad.ResetVibration();
			}

			if (gamepad.IsButtonPressed(Button::BACK))
				break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	std::cin.get();
	return 0;
}

inline static auto Clear() -> void
{
#ifdef _WIN32 || _WIN64
	std::system("cls");
#else
	std::system("Clear");
#endif
}
