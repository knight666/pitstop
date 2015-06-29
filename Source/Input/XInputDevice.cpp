#include "Input/XInputDevice.h"

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

}; // namespace Pitstop