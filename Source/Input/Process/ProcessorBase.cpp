#include "Input/Process/ProcessorBase.h"

#include "Input/RawInputJoystick.h"

namespace Pitstop {

	ProcessorBase::ProcessorBase(RawInputJoystick& joystick)
		: m_Joystick(joystick)
	{
	}

	ProcessorBase::~ProcessorBase()
	{
		delete [] m_PreparsedData;
	}

	bool ProcessorBase::setup()
	{
		UINT preparsed_size = 0;
		if (::GetRawInputDeviceInfoW(m_Joystick.getHandle(), RIDI_PREPARSEDDATA, nullptr, &preparsed_size) != 0)
		{
			DWORD errorCode = GetLastError();

			return false;
		}

		m_PreparsedData = new uint8_t[preparsed_size];
		memset(m_PreparsedData, 0, preparsed_size);
		if (::GetRawInputDeviceInfoW(m_Joystick.getHandle(), RIDI_PREPARSEDDATA, m_PreparsedData, &preparsed_size) == (UINT)-1)
		{
			DWORD errorCode = GetLastError();

			return false;
		}

		m_Preparsed = (PHIDP_PREPARSED_DATA)m_PreparsedData;

		if (::HidP_GetCaps(m_Preparsed, &m_Capabilities) != HIDP_STATUS_SUCCESS)
		{
			DWORD errorCode = GetLastError();

			return false;
		}

		m_ButtonCapabilities.resize(m_Capabilities.NumberInputButtonCaps);
		USHORT button_count = m_Capabilities.NumberInputButtonCaps;
		if (::HidP_GetButtonCaps(HidP_Input, &m_ButtonCapabilities[0], &button_count, m_Preparsed) != HIDP_STATUS_SUCCESS)
		{
			DWORD errorCode = GetLastError();

			return false;
		}

		m_ValueCapabilities.resize(m_Capabilities.NumberInputButtonCaps);
		USHORT value_count = m_Capabilities.NumberInputValueCaps;
		if (::HidP_GetValueCaps(HidP_Input, &m_ValueCapabilities[0], &value_count, m_Preparsed) != HIDP_STATUS_SUCCESS)
		{
			DWORD errorCode = GetLastError();

			return false;
		}

		return true;
	}

}; // namespace Pitstop