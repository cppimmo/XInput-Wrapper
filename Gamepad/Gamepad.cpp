#include "Gamepad.h"
#include <math.h>
#include <algorithm>
#include <climits>

Gamepad::Gamepad(UINT id) : controllerID(id),
	deadzoneX(XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE),
	deadzoneY(XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
{
	ZeroMemory(&state, sizeof(XINPUT_STATE));
	ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
}

Gamepad::Gamepad(UINT id, float deadzoneX, float deadzoneY)
	: deadzoneX(deadzoneX), deadzoneY(deadzoneY)
{
	ZeroMemory(&state, sizeof(XINPUT_STATE));
	ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
}

UINT Gamepad::getControllerID() const
{
	return controllerID;
}

XINPUT_GAMEPAD* Gamepad::getGamepad()
{
	return &state.Gamepad;
}

XINPUT_BATTERY_INFORMATION* Gamepad::getBatteryInfo()
{
	//XInputGetBatteryInformation(controllerID, XINPUT_DEVTYPE_GAMEPAD, &battery);
	return &battery;
}

bool Gamepad::isConnected()
{
	if (XInputGetState(controllerID, &state) == ERROR_SUCCESS)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Gamepad::Update()
{
	if (!isConnected())
		return false;

	//leftStickX = ApplyDeadzone(state.Gamepad.sThumbLX, maxValue, 0.024f);
	//leftStickY = ApplyDeadzone(state.Gamepad.sThumbLY, maxValue, 0.024f);

	//rightStickX = ApplyDeadzone(state.Gamepad.sThumbRX, maxValue, 0.024f);
	//rightStickY = ApplyDeadzone(state.Gamepad.sThumbRY, maxValue, 0.024f);
	
	//float LX = state.Gamepad.sThumbLX;
	float LX = normalize(static_cast<float>(state.Gamepad.sThumbLX), -32767.0f, 32767.0f);
	float LY = normalize(static_cast<float>(state.Gamepad.sThumbLY), -32767, 32767);

	float RX = normalize(static_cast<float>(state.Gamepad.sThumbRX), -32767, 32767);
	float RY = normalize(static_cast<float>(state.Gamepad.sThumbRY), -32767, 32767);

	leftStickX = ApplyDeadzone(LX, 1.f, normalize(deadzoneX, SHRT_MIN, SHRT_MAX));
	leftStickY = ApplyDeadzone(LY, 1.f, normalize(deadzoneY, SHRT_MIN, SHRT_MAX));
	rightStickX = ApplyDeadzone(RX, 1.f, normalize(deadzoneX, SHRT_MIN, SHRT_MAX));
	rightStickY = ApplyDeadzone(RY, 1.f, normalize(deadzoneY, SHRT_MIN, SHRT_MAX));
	//leftStickY = state.Gamepad.sThumbLY;
	//rightStickX = state.Gamepad.sThumbRX;
	//rightStickY = state.Gamepad.sThumbRY;

	leftTrigger = static_cast<float>(state.Gamepad.bLeftTrigger) / 255.0f;//normalize input 
	rightTrigger = static_cast<float>(state.Gamepad.bRightTrigger) / 255.0f;
	return true;
}

void Gamepad::Vibrate(USHORT leftSpeed, USHORT rightSpeed)
{
	vibration.wLeftMotorSpeed = leftSpeed;
	vibration.wRightMotorSpeed = rightSpeed;
	XInputSetState(controllerID, &vibration);
}

void Gamepad::Vibrate(USHORT speed)
{
	vibration.wLeftMotorSpeed = speed;
	vibration.wRightMotorSpeed = speed / 2;
	XInputSetState(controllerID, &vibration);
}

void Gamepad::resetVibration()
{
	vibration.wLeftMotorSpeed = 0;
	vibration.wRightMotorSpeed = 0;
	XInputSetState(controllerID, &vibration);
}

bool Gamepad::isButtonPressed(UINT button) const
{
	return (state.Gamepad.wButtons & button) != 0;
}

float Gamepad::ApplyDeadzone(float value, float maxValue, float deadzone)
{
	if (value < -deadzone)
	{
		value += deadzone; //increase neg vals to remove deadzone discontinuity
	}
	else if (value > deadzone)
	{
		value -= deadzone; //decrease pos vals to remove deadzone discontinuity
	}
	else
	{
		return 0; //hey values are zero for once
	}
	//float normDeadzone = (deadzone - (-32767)) / (32767 - (-32767));
	float normValue = (float)value / (float)(maxValue - deadzone);
	return std::max(-1.0f, std::min(normValue, 1.0f));
}

float normalize(float input, float min, float max)
{
	float average = (min + max) / 2;
	float range = (max - min) / 2;
	return (input - average) / range;
}