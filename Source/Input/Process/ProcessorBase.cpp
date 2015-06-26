#include "Input/Process/ProcessorBase.h"

#include "Input/RawInputJoystick.h"

namespace Pitstop {

	ProcessorBase::ProcessorBase(RawInputJoystick& joystick)
		: m_Joystick(joystick)
	{
	}

	ProcessorBase::~ProcessorBase()
	{
	}

	bool ProcessorBase::setup()
	{
		UINT preparsed_size = 0;
		if (::GetRawInputDeviceInfoW(
			m_Joystick.getHandle(),
			RIDI_PREPARSEDDATA,
			nullptr,
			&preparsed_size) != 0)
		{
			DWORD errorCode = GetLastError();

			return false;
		}

		m_PreparsedData.resize(preparsed_size);
		if (::GetRawInputDeviceInfoW(
			m_Joystick.getHandle(),
			RIDI_PREPARSEDDATA,
			&m_PreparsedData[0],
			&preparsed_size) == (UINT)-1)
		{
			DWORD errorCode = GetLastError();

			return false;
		}

		m_Preparsed = (PHIDP_PREPARSED_DATA)&m_PreparsedData[0];

		if (::HidP_GetCaps(
			m_Preparsed,
			&m_Capabilities) != HIDP_STATUS_SUCCESS)
		{
			DWORD errorCode = GetLastError();

			return false;
		}

		m_ButtonCapabilities.resize(m_Capabilities.NumberInputButtonCaps);
		USHORT button_caps_count = m_Capabilities.NumberInputButtonCaps;
		if (::HidP_GetButtonCaps(
			HidP_Input,
			&m_ButtonCapabilities[0],
			&button_caps_count,
			m_Preparsed) != HIDP_STATUS_SUCCESS)
		{
			DWORD errorCode = GetLastError();

			return false;
		}

		USAGE button_count = 0;
		for (HIDP_BUTTON_CAPS& buttons : m_ButtonCapabilities)
		{
			button_count += buttons.Range.UsageMax - buttons.Range.UsageMin + 1;
		}
		m_ButtonState.resize(button_count);

		m_ValueCapabilities.resize(m_Capabilities.NumberInputValueCaps);
		USHORT value_caps_count = m_Capabilities.NumberInputValueCaps;
		if (::HidP_GetValueCaps(
			HidP_Input,
			&m_ValueCapabilities[0],
			&value_caps_count,
			m_Preparsed) != HIDP_STATUS_SUCCESS)
		{
			DWORD errorCode = GetLastError();

			return false;
		}

		for (HIDP_VALUE_CAPS& values : m_ValueCapabilities)
		{
			m_AxisValues[values.Range.UsageMin] = 0;
		}

		return true;
	}

	bool ProcessorBase::process(const RAWINPUT& message)
	{
		// Save previous button state and compare

		QVector<USAGE> button_state_previous(m_ButtonState.size());
		for (int i = 0; i < m_ButtonState.size(); ++i)
		{
			button_state_previous[i] = m_ButtonState[i];
			m_ButtonState[i] = InputState_Up;
		}

		// Read button state

		size_t buttons_index = 0;
		for (HIDP_BUTTON_CAPS& buttons : m_ButtonCapabilities)
		{
			QVector<USAGE> usage_list(m_ButtonState.size());
			ULONG usage_length = m_ButtonState.size();

			NTSTATUS success = ::HidP_GetUsages(
				HidP_Input,
				buttons.UsagePage,
				0,
				&usage_list[0],
				&usage_length,
				m_Preparsed,
				(PCHAR)message.data.hid.bRawData,
				message.data.hid.dwSizeHid);
			if (success != HIDP_STATUS_SUCCESS ||
				usage_length == 0)
			{
				continue;
			}

			for (ULONG i = 0; i < usage_length; ++i)
			{
				USAGE index = usage_list[i] - buttons.Range.UsageMin;

				if ((button_state_previous[index] & InputState_Down) == 0)
				{
					m_ButtonState[index] = InputState_Pressed;
				}
				m_ButtonState[index] |= InputState_Down;
			}
		}

		// Read axis values

		size_t values_index = 0;
		for (HIDP_VALUE_CAPS& values : m_ValueCapabilities)
		{
			LONG value = 0;

			NTSTATUS success = ::HidP_GetUsageValue(
				HidP_Input,
				values.UsagePage,
				0,
				values.Range.UsageMin,
				(PULONG)&value,
				m_Preparsed,
				(PCHAR)message.data.hid.bRawData,
				message.data.hid.dwSizeHid);
			if (success != HIDP_STATUS_SUCCESS)
			{
				break;
			}

			m_AxisValues[values.Range.UsageMin] = value;
		}

		// Process input

		for (int i = 0; i < m_ButtonState.size(); ++i)
		{
			if ((button_state_previous[i] & InputState_Down) != 0 &&
				(m_ButtonState[i] & InputState_Down) == 0)
			{
				m_ButtonState[i] = InputState_Released;
			}

			processButtonState(buttons_index, m_ButtonState[i]);
		}

		for (QHash<USAGE, LONG>::iterator it = m_AxisValues.begin(); it != m_AxisValues.end(); ++it)
		{
			processAxisState(it.key(), it.value());
		}

		return true;
	}

	bool ProcessorBase::processButtonState(size_t index, uint8_t state)
	{
		return true;
	}

	bool ProcessorBase::processAxisState(size_t index, LONG value)
	{
		return true;
	}

}; // namespace Pitstop