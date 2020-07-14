#pragma once

#define NOMINMAX
#include <Windows.h>
#include <Xinput.h>

class Gamepad
{
private:
	UINT controllerID;
	XINPUT_STATE state;
	XINPUT_VIBRATION vibration;
	XINPUT_BATTERY_INFORMATION battery;
	
	const float maxValue = 1.0f;

	float deadzoneX;
	float deadzoneY;
public:
	Gamepad(UINT id);
	Gamepad(UINT id, float deadzoneX, float deadzoneY);

	inline UINT getControllerID() const;
	XINPUT_GAMEPAD* getGamepad();
	//XInputGetBatteryInformatioin is not supported for 9.1.0
	//XINPUT_BATTERY_INFORMATION* getBatteryInfo();
	
	float leftStickX, leftStickY;
	float rightStickX, rightStickY;
	float leftTrigger, rightTrigger;

	bool isConnected();
	bool Update();
	void Vibrate(USHORT leftSpeed, USHORT rightSpeed);
	void Vibrate(USHORT speed);
	void resetVibration();
	bool isButtonPressed(UINT button) const;
private:
	float ApplyDeadzone(float value, float maxValue, float deadzone);
};
