#include "Input/Process/InputProcessorBase.h"

#include "Input/RawInput/RawInputJoystick.h"

namespace Pitstop {

	InputProcessorBase::InputProcessorBase(RawInputJoystick& joystick)
		: m_Joystick(joystick)
	{
	}

	InputProcessorBase::~InputProcessorBase()
	{
	}

	bool InputProcessorBase::setup()
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

		// Setup digital input

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
			for (USAGE button = buttons.Range.UsageMin; button < buttons.Range.UsageMax; ++button)
			{
				m_ButtonsState[button] = false;
			}
		}

		// Setup analog axes

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

		// Add bindings

		createBindings();

		return true;
	}

	bool InputProcessorBase::process(const RAWINPUT& message)
	{
		// Clear previous state

		for (QHash<USAGE, bool>::iterator it = m_ButtonsState.begin(); it != m_ButtonsState.end(); ++it)
		{
			it.value() = false;
		}

		// Read button state

		size_t buttons_index = 0;
		for (HIDP_BUTTON_CAPS& buttons : m_ButtonCapabilities)
		{
			QVector<USAGE> usage_list(m_ButtonsState.size());
			ULONG usage_length = m_ButtonsState.size();

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
				m_ButtonsState[usage_list[i]] = true;
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

		for (QHash<USAGE, bool>::iterator it = m_ButtonsState.begin(); it != m_ButtonsState.end(); ++it)
		{
			processDigital(it.key(), it.value());
		}

		for (QHash<USAGE, LONG>::iterator it = m_AxisValues.begin(); it != m_AxisValues.end(); ++it)
		{
			processAnalog(it.key(), it.value());
		}

		return true;
	}

	void InputProcessorBase::addBinding(const QString& name, InputType type)
	{
		m_Bindings[name] = InputBinding(name, m_Bindings.size(), type);
	}

}; // namespace Pitstop