#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#pragma comment(lib, "xinput.lib")
#include <XInput.h>
#include <queue>
#include <string>
#include <optional>

class Gamepad
{
public:
	enum class Button : unsigned int
	{
		DPAD_UP = XINPUT_GAMEPAD_DPAD_UP,
		DPAD_DOWN = XINPUT_GAMEPAD_DPAD_DOWN,
		DPAD_LEFT = XINPUT_GAMEPAD_DPAD_LEFT,
		DPAD_RIGHT = XINPUT_GAMEPAD_DPAD_RIGHT,
		START = XINPUT_GAMEPAD_START,
		BACK = XINPUT_GAMEPAD_BACK,
		LEFT_THUMB = XINPUT_GAMEPAD_LEFT_THUMB,
		RIGHT_THUMB = XINPUT_GAMEPAD_RIGHT_THUMB,
		LEFT_SHOULDER = XINPUT_GAMEPAD_LEFT_SHOULDER,
		RIGHT_SHOULDER = XINPUT_GAMEPAD_RIGHT_SHOULDER,
		A = XINPUT_GAMEPAD_A,
		B = XINPUT_GAMEPAD_B,
		X = XINPUT_GAMEPAD_X,
		Y = XINPUT_GAMEPAD_Y,
	};
	class ButtonEvent
	{
	public:
		enum class Type
		{
			PRESS,
			RELEASE,
		};
	private:
		Type type;
		Button button;
	public:
		ButtonEvent( Type type, Button button ) noexcept
			: 
			type( type ),
			button ( button )
		{}
		bool IsPress() const noexcept
		{
			return type == Type::PRESS;
		}
		bool IsRelease() const noexcept
		{
			return type == Type::RELEASE;
		}
		Button getButton() const noexcept
		{
			return button;
		}
	};
	struct Axis
	{
		float x, y;
	};
	using Deadzone = Axis;
public:
	// id is a number of 1-4 but is represented by 0-3 in the API
	Gamepad(const unsigned int id, Deadzone deadzone = { XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE,
		XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE } );
	Gamepad( const Gamepad& ) = delete;
	Gamepad& operator=( const Gamepad& ) = delete;

	const UINT GetControllerID() const noexcept;
	XINPUT_GAMEPAD* const GetGamepad();
	bool IsConnected();
	bool Update();
	void Vibrate( unsigned short leftSpeed, unsigned short rightSpeed );
	void Vibrate( unsigned short bothSpeed );
	void ResetVibration();
	bool IsButtonPressed( Button button ) const;
	std::optional<ButtonEvent> ReadButtonBuffer() noexcept;
	Axis &LeftStick() noexcept;
	Axis &RightStick() noexcept;
	float LeftTrigger() const noexcept;
	float RightTrigger() const noexcept;
	bool GetAudioDeviceIDs( const std::wstring& pRenderDeviceId, unsigned int* pRenderCount,
		const std::wstring& pCaptureDeviceId, unsigned int* pCaptureCount ) const;
	XINPUT_CAPABILITIES* const GetCapabilities( const unsigned long flags = 0u );
	void SetDeadzone( const Deadzone deadzone ) noexcept;
	Deadzone& GetDeadzone() noexcept;
	bool ButtonIsEmpty() const noexcept;
	void Flush() noexcept;
	// XInputGetBatteryInformation is not supported for 9.1.0
	// XINPUT_BATTERY_INFORMATION* getBatteryInfo();
	// void Enable(bool value) const; no supported for 9.1.0
private: // axis & trigger values
	Axis leftStick;
	Axis rightStick;
	float leftTrigger, rightTrigger;
private:
	void OnButtonPressed( Button button ) noexcept;
	void OnButtonReleased( Button button ) noexcept;
	static float ApplyDeadzone( float value, const float maxValue, 
		const float deadzone );
	template <typename buf>
	static void TrimBuffer( std::queue<buf> &buffer ) noexcept;
private:
	static constexpr unsigned int bufferSize = 16u;
	static constexpr float maxAxisValue = 1.0f;
	static constexpr float triggerThreshold = XINPUT_GAMEPAD_TRIGGER_THRESHOLD / 255.0f;

	const unsigned int controllerID;
	Deadzone deadzone;

	XINPUT_STATE state;
	XINPUT_VIBRATION vibration;
	XINPUT_BATTERY_INFORMATION battery;
	XINPUT_CAPABILITIES capabilities;

	std::queue<ButtonEvent> buttonbuffer;
};
