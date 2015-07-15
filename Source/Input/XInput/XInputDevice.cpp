#include "Input/XInput/XInputDevice.h"

namespace Pitstop {

	XInputDevice::XInputDevice(DWORD userIndex)
		: m_UserIndex(userIndex)
		, m_Connected(false)
		, m_LastPacket(0)
	{
	}

	XInputDevice::~XInputDevice()
	{
	}

	void XInputDevice::updateState(XINPUT_STATE& state, bool connected)
	{
		m_State = state.Gamepad;
		m_LastPacket = state.dwPacketNumber;
		m_Connected = connected;

		emit signalChanged();
	}

}; // namespace Pitstop