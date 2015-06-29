#include "Input/XInputDevice.h"

#include "Input/Process/ProcessorBase.h"
#include "Input/RawInputJoystick.h"

namespace Pitstop {

	XInputDevice::XInputDevice(size_t controllerIndex)
		: m_ControllerIndex(controllerIndex)
		, m_Joystick(nullptr)
		, m_DeviceHandle(NULL)
		, m_PluggedIn(false)
	{
	}

	XInputDevice::~XInputDevice()
	{
	}

	bool XInputDevice::attach(RawInputJoystick& joystick)
	{
		if (m_DeviceHandle != NULL)
		{
			::CloseHandle(m_DeviceHandle);
		}

		m_DeviceHandle = ::CreateFileW(
			joystick.getDevicePath().utf16(),
			GENERIC_WRITE | GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			nullptr,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
			0);
		if (m_DeviceHandle == NULL)
		{
			return false;
		}

		m_Joystick = &joystick;

		return true;
	}

	void XInputDevice::detach()
	{
		if (m_DeviceHandle != NULL)
		{
			::CloseHandle(m_DeviceHandle);
			m_DeviceHandle = NULL;
		}

		m_Joystick = nullptr;
	}

	void XInputDevice::setPluggedIn(bool value)
	{
		if (!isActive() ||
			value == m_PluggedIn)
		{
			return;
		}

		DWORD command = value ? 0x002A4000 : 0x002A4004;
		
		uint8_t buffer[16] = { 0 };
		buffer[0] = 0x10;
		buffer[4] = (uint8_t)m_ControllerIndex + 1;

		DWORD written = 0;

		BOOL result = ::DeviceIoControl(
			m_DeviceHandle,
			command,
			buffer, 16,
			nullptr, 0,
			&written,
			nullptr);
	}

	void XInputDevice::writeOutput(const XInputState& state)
	{
		uint8_t input[28] = { 0 };
		uint8_t output[8] = { 0 };

		input[ 0] = 0x1C;
		input[ 4] = (uint8_t)m_ControllerIndex + 1;
		input[ 9] = 0x14;

		for (size_t i = 0; i < (size_t)XInputState::Button::_COUNT; ++i)
		{
			if ((state.buttonState[i] & ProcessorBase::InputState_Down) != 0)
			{
				input[10 + (i / 7)] = (uint8_t)(1 << i);
			}
		}

		input[12] = (uint8_t)(state.axisState[(size_t)XInputState::Axis::LeftTrigger] * 255.0f);
		input[13] = (uint8_t)(state.axisState[(size_t)XInputState::Axis::RightTrigger] * 255.0f);

		// TODO: Sticks

		DWORD written = 0;

		if (::DeviceIoControl(
			m_DeviceHandle,
			0x2A400C,
			input, 28,
			output, 8,
			&written,
			nullptr) == FALSE)
		{
			DWORD last_error = ::GetLastError();
		}
	}

}; // namespace Pitstop