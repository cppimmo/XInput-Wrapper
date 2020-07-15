#include "Gamepad.h"
#include <math.h>
#include <algorithm>
#include <climits>

float normalize(float input, float min, float max);

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

/*XINPUT_BATTERY_INFORMATION* Gamepad::getBatteryInfo()
{
	XInputGetBatteryInformation(controllerID, XINPUT_DEVTYPE_GAMEPAD, &battery);
	return &battery;
}*/

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

	float normLX = normalize(static_cast<float>(state.Gamepad.sThumbLX), -32767, 32767);
	float normLY = normalize(static_cast<float>(state.Gamepad.sThumbLY), -32767, 32767);

	float normRX = normalize(static_cast<float>(state.Gamepad.sThumbRX), -32767, 32767);
	float normRY = normalize(static_cast<float>(state.Gamepad.sThumbRY), -32767, 32767);

	leftStickX = ApplyDeadzone(normLX,  maxValue, normalize(deadzoneX, SHRT_MIN, SHRT_MAX));
	leftStickY = ApplyDeadzone(normLY, maxValue, normalize(deadzoneY, SHRT_MIN, SHRT_MAX));
	rightStickX = ApplyDeadzone(normRX, maxValue, normalize(deadzoneX, SHRT_MIN, SHRT_MAX));
	rightStickY = ApplyDeadzone(normRY, maxValue, normalize(deadzoneY, SHRT_MIN, SHRT_MAX));

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
	float normValue = (float)value / (float)(maxValue - deadzone);
	return std::max(-1.0f, std::min(normValue, 1.0f));
}

float normalize(float input, float min, float max)
{
	float average = (min + max) / 2;
	float range = (max - min) / 2;
	return (input - average) / range;
}