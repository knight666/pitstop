#include "DriverBase.h"

#include <Input/RawInput/RawInputJoystick.h>

namespace Pitstop {

	DriverBase::DriverBase()
	{
	}

	DriverBase::~DriverBase()
	{
	}

	bool DriverBase::setup(RawInputJoystick* joystick)
	{
		m_Joystick = joystick;

		UINT preparsed_size = 0;
		if (::GetRawInputDeviceInfoW(
			m_Joystick->getHandle(),
			RIDI_PREPARSEDDATA,
			nullptr,
			&preparsed_size) != 0)
		{
			PS_LOG_ERROR(InputProcessor) << "Failed to retrieve preparsed data from joystick." << PS_LOG_WINDOWS_ERROR;

			return false;
		}

		m_PreparsedData.resize(preparsed_size);

		if (::GetRawInputDeviceInfoW(
			m_Joystick->getHandle(),
			RIDI_PREPARSEDDATA,
			&m_PreparsedData[0],
			&preparsed_size) == (UINT)-1)
		{
			PS_LOG_ERROR(InputProcessor) << "Failed to retrieve preparsed data from joystick." << PS_LOG_WINDOWS_ERROR;

			return false;
		}

		m_Preparsed = (PHIDP_PREPARSED_DATA)&m_PreparsedData[0];

		if (::HidP_GetCaps(
			m_Preparsed,
			&m_Capabilities) != HIDP_STATUS_SUCCESS)
		{
			PS_LOG_ERROR(InputProcessor) << "Failed to get joystick capabilities." << PS_LOG_WINDOWS_ERROR;

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
			PS_LOG_ERROR(InputProcessor) << "Failed to retrieve get joystick button capabilities." << PS_LOG_WINDOWS_ERROR;

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
			PS_LOG_ERROR(InputProcessor) << "Failed to retrieve joystick axis capabilities." << PS_LOG_WINDOWS_ERROR;

			return false;
		}

		for (HIDP_VALUE_CAPS& values : m_ValueCapabilities)
		{
			m_AxisValues[values.Range.UsageMin] = 0;
		}

		return true;
	}

	bool DriverBase::process(const RAWINPUT& message)
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

	void DriverBase::addDigitalInput(const char* name, USAGE identifier)
	{
		DigitalInput input;
		strcpy_s(input.name, name);
		input.identifier = identifier;
		input.value = false;

		m_DigitalInputs[identifier] = input;
	}

	void DriverBase::addAnalogInput(const char* name, USAGE identifier, float minimum, float maximum)
	{
		AnalogInput input;
		strcpy_s(input.name, name);
		input.identifier = identifier;
		input.minimum = minimum;
		input.maximum = maximum;
		input.value = 0.0f;

		m_AnalogInputs[identifier] = input;
	}

	bool DriverBase::processDigital(USAGE identifier, bool value)
	{
		auto found = m_DigitalInputs.find(identifier);
		if (found != m_DigitalInputs.end())
		{
			found.value().value = value;

			return true;
		}

		return false;
	}

	bool DriverBase::processAnalog(USAGE identifier, LONG value)
	{
		auto found = m_AnalogInputs.find(identifier);
		if (found != m_AnalogInputs.end())
		{
			auto& input = found.value();
			input.value = (static_cast<float>(value) - input.minimum) / (input.maximum - input.minimum);

			return true;
		}

		return false;
	}

};