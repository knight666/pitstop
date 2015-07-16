#include "Input/XInput/XInputDevice.h"

namespace Pitstop {

	XInputDevice::XInputDevice(DWORD userIndex)
		: m_UserIndex(userIndex)
		, m_Connected(false)
		, m_Activated(false)
		, m_LastPacket(0)
	{
		memset(&m_State, 0, sizeof(m_State));
	}

	XInputDevice::~XInputDevice()
	{
	}

	void XInputDevice::updateState(XINPUT_STATE& state, bool connected)
	{
		static const uint32_t left_stick_deadzone_magnitude =
			XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE * XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;

		uint32_t left_stick_magnitude =
			(state.Gamepad.sThumbLX * state.Gamepad.sThumbLX) +
			(state.Gamepad.sThumbLY * state.Gamepad.sThumbLY);

		static const uint32_t right_stick_deadzone_magnitude =
			XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE * XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;

		uint32_t right_stick_magnitude =
			(state.Gamepad.sThumbRX * state.Gamepad.sThumbRX) +
			(state.Gamepad.sThumbRY * state.Gamepad.sThumbRY);

		m_Activated =
			(state.Gamepad.wButtons != 0) ||
			(state.Gamepad.bLeftTrigger >= XINPUT_GAMEPAD_TRIGGER_THRESHOLD) ||
			(state.Gamepad.bRightTrigger >= XINPUT_GAMEPAD_TRIGGER_THRESHOLD) ||
			(left_stick_magnitude >= left_stick_deadzone_magnitude) ||
			(right_stick_magnitude >= right_stick_deadzone_magnitude);

		m_State = state.Gamepad;
		m_LastPacket = state.dwPacketNumber;
		m_Connected = connected;

		emit signalChanged();
	}

}; // namespace Pitstop