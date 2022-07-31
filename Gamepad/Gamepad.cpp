#include "Gamepad.h"
#include <algorithm>
#include <climits>
#include <cmath>

static float Normalize( float value, float min, float max );

Gamepad::Gamepad( unsigned int id, const Deadzone &deadzone )
	: controllerID( id ), deadzone( deadzone )
{
	ZeroMemory( &state, sizeof(XINPUT_STATE) );
	ZeroMemory( &vibration, sizeof(XINPUT_VIBRATION) );
	ZeroMemory( &battery, sizeof(XINPUT_BATTERY_INFORMATION) );
	ZeroMemory( &capabilities, sizeof(XINPUT_CAPABILITIES) );
}

UINT Gamepad::GetControllerID() const noexcept
{
	return this->controllerID - 1;
}

XINPUT_GAMEPAD* Gamepad::GetGamepad() noexcept
{
	return &this->state.Gamepad;
}

const XINPUT_GAMEPAD *Gamepad::GetGamepad() const noexcept
{
	return &this->state.Gamepad;
}

/*XINPUT_BATTERY_INFORMATION* Gamepad::getBatteryInfo()
{
	XInputGetBatteryInformation(controllerID, XINPUT_DEVTYPE_GAMEPAD, &battery);
	return &battery;
}*/

bool Gamepad::IsConnected()
{
	return ( XInputGetState( this->controllerID - 1, &state ) == ERROR_SUCCESS )
		? true : false;
}

bool Gamepad::Update()
{
	if (!IsConnected())
		return false; // don't update the controller if it's not connected

	if (state.Gamepad.wButtons != 0u) 
	{
		OnButtonPressed( static_cast<Gamepad::Button>(state.Gamepad.wButtons) );
	}
	else
	{
		if (!ButtonIsEmpty())
			OnButtonReleased( buttonbuffer.front().getButton() );
	}

	static constexpr std::numeric_limits<short> limitShrt{};
	static constexpr auto limitMin = limitShrt.min();
	static constexpr auto limitMax = limitShrt.max();
	// Normalize input from the left and right stick's x and y axes.
	const float normLX = Normalize( static_cast<float>(state.Gamepad.sThumbLX),
								   limitMin, limitMax);
	const float normLY = Normalize( static_cast<float>(state.Gamepad.sThumbLY),
								   limitMin, limitMax);

	const float normRX = Normalize( static_cast<float>(state.Gamepad.sThumbRX),
								   limitMin, limitMax);
	const float normRY = Normalize( static_cast<float>(state.Gamepad.sThumbRY),
								   limitMin, limitMax);
	
	// Apply the deadzones to the normalized form of each stick axes.
	leftStick.x = ApplyDeadzone(  normLX, maxAxisValue, Normalize(deadzone.x, limitMin, limitMax) );
	leftStick.y = ApplyDeadzone(  normLY, maxAxisValue, Normalize(deadzone.y, limitMin, limitMax) );
	rightStick.x = ApplyDeadzone( normRX, maxAxisValue, Normalize(deadzone.x, limitMin, limitMax) );
	rightStick.y = ApplyDeadzone( normRY, maxAxisValue, Normalize(deadzone.y, limitMin, limitMax) );
	// Normalize trigger input of BYTE/char limit.
	static constexpr std::numeric_limits<unsigned char> limitChr{};
	leftTrigger = (
		static_cast<float>(state.Gamepad.bLeftTrigger)
		/ static_cast<float>(limitChr.max())
	);
	rightTrigger = (
		static_cast<float>(state.Gamepad.bRightTrigger)
		/ static_cast<float>(limitChr.max())
	);
	return true;
}

/*void Gamepad::Enable(bool value) const
{
	// XInputEnable is meant for use when an application loses/gains focus
	// for example the WM_ACTIVATEAPP message.
	// you may pass FALSE when you wish for vibrations to stop and vice versa
	XInputEnable(static_cast<BOOL>(value));
}*/

void Gamepad::Vibrate( unsigned short leftSpeed, unsigned short rightSpeed )
{
	vibration.wLeftMotorSpeed = leftSpeed;
	vibration.wRightMotorSpeed = rightSpeed;
	XInputSetState( GetControllerID(), &vibration );
}

void Gamepad::Vibrate( unsigned short lrSpeed )
{
	vibration.wLeftMotorSpeed = lrSpeed;
	vibration.wRightMotorSpeed = lrSpeed / 2u;
	XInputSetState( GetControllerID(), &vibration );
}

void Gamepad::ResetVibration()
{
	vibration.wLeftMotorSpeed = 0u;
	vibration.wRightMotorSpeed = 0u;
	XInputSetState(GetControllerID(), &vibration);
}

bool Gamepad::IsButtonPressed( Button button ) const
{
	return ( state.Gamepad.wButtons & static_cast<unsigned short>(button) ) != 0u;
}

std::optional<Gamepad::ButtonEvent> Gamepad::ReadButtonBuffer() noexcept
{
	if ( buttonbuffer.size() > 0u )
	{
		ButtonEvent e = buttonbuffer.front();
		buttonbuffer.pop();
		return e;
	}
	return {};
}

Gamepad::Axis& Gamepad::LeftStick() noexcept
{
	return leftStick;
}

const Gamepad::Axis& Gamepad::LeftStick() const noexcept
{
	return leftStick;
}

Gamepad::Axis& Gamepad::RightStick() noexcept
{
	return rightStick;
}

const Gamepad::Axis& Gamepad::RightStick() const noexcept
{
	return rightStick;
}

float Gamepad::LeftTrigger() const noexcept
{
	return leftTrigger;
}

float Gamepad::RightTrigger() const noexcept
{
	return rightTrigger;
}

bool Gamepad::GetAudioDeviceIDs( const std::wstring& pRenderDeviceId, unsigned int* pRenderCount,
	const std::wstring& pCaptureDeviceId, unsigned int* pCaptureCount ) const
{ // I'm not quite sure how to use this one but this should wrap it okay
	const auto result = XInputGetAudioDeviceIds( GetControllerID(),
		const_cast<wchar_t*>(pRenderDeviceId.c_str()), pRenderCount, 
		const_cast<wchar_t*>(pCaptureDeviceId.c_str()), pCaptureCount );
	if (result != ERROR_SUCCESS)
		return false;
	return true;
}

XINPUT_CAPABILITIES* Gamepad::GetCapabilities( unsigned long flags )
{ // returns const pointer to XINPUT_CAPABILITIES structure that details  
  // useful charateristics about the controller with the dwUserIndex parameter
  // you may pass XINPUT_FLAG_GAMEPAD as the value of flags to limit use to Xbox 360 controllers
	const auto result = XInputGetCapabilities(GetControllerID(), flags,
											  &this->capabilities);
	return &this->capabilities;
}

const XINPUT_CAPABILITIES* Gamepad::GetCapabilities( unsigned long flags ) const
{
	const auto result = XInputGetCapabilities(GetControllerID(), flags,
											  &this->capabilities);
	return &this->capabilities;
}

void Gamepad::SetDeadzone( const Deadzone& deadzone ) noexcept
{
	this->deadzone = deadzone;
}

void Gamepad::OnButtonPressed( Button button ) noexcept
{
	using ButtonEventType = ButtonEvent::Type;
	buttonbuffer.push( ButtonEvent( ButtonEventType::PRESS,button ) );
	TrimBuffer( buttonbuffer );
}

void Gamepad::OnButtonReleased( Button button ) noexcept
{
	using ButtonEventType = ButtonEvent::Type;
	buttonbuffer.push( ButtonEvent( ButtonEventType::RELEASE,button ) );
	TrimBuffer( buttonbuffer );
}

Gamepad::Deadzone& Gamepad::GetDeadzone() noexcept
{
	return deadzone;
}

const Gamepad::Deadzone& Gamepad::GetDeadzone() const noexcept
{
	return deadzone;
}

inline bool Gamepad::ButtonIsEmpty() const noexcept
{
	return buttonbuffer.empty();
}

inline void Gamepad::Flush() noexcept
{
	buttonbuffer = std::queue<Gamepad::ButtonEvent>();
}

float Gamepad::ApplyDeadzone( float value, const float maxValue, float deadzone)
{
	if (value < -(deadzone))
	{
		value += deadzone; // increase neg vals to remove deadzone discontinuity
	}
	else if (value > deadzone)
	{
		value -= deadzone; // decrease pos vals to remove deadzone discontinuity
	}
	else
	{
		return 0.0f; // hey values are zero for once
	}
	const float normValue = value / (maxValue - deadzone);
	return std::max(-1.0f, std::min(normValue, 1.0f));
}

template<typename buf>
inline void Gamepad::TrimBuffer(std::queue<buf> &buffer) noexcept
{
	while ( buffer.size() > bufferSize )
	{
		buffer.pop(); // [[yesdiscard]] xD
	}
}

static float Normalize(float value, float min, float max)
{ // Normalize value between min/max range.
	const float average = (min + max) / 2.0f;
	const float range = (max - min) / 2.0f;
	return (value - average) / range;
}