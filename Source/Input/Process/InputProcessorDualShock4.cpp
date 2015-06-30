#include "Input/Process/InputProcessorDualShock4.h"

namespace Pitstop {

	void InputProcessorDualShock4::createBindings()
	{
		m_LeftStickValues[0] = 0;
		m_LeftStickValues[1] = 0;

		m_RightStickValues[0] = 0;
		m_RightStickValues[1] = 0;

		addBinding("Square", InputType::Digital);
		addBinding("Cross", InputType::Digital);
		addBinding("Circle", InputType::Digital);
		addBinding("Triangle", InputType::Digital);
		addBinding("L1", InputType::Digital);
		addBinding("R1", InputType::Digital);
		addBinding("L2", InputType::Digital);
		addBinding("R2", InputType::Digital);
		addBinding("Share", InputType::Digital);
		addBinding("Options", InputType::Digital);
		addBinding("LeftThumb", InputType::Digital);
		addBinding("RightThumb", InputType::Digital);
		addBinding("Guide", InputType::Digital);
		addBinding("Up", InputType::Digital);
		addBinding("Right", InputType::Digital);
		addBinding("Down", InputType::Digital);
		addBinding("Left", InputType::Digital);

		addBinding("LeftStickHorizontal", InputType::Analog);
		addBinding("LeftStickVertical", InputType::Analog);
		addBinding("RightStickHorizontal", InputType::Analog);
		addBinding("RightStickVertical", InputType::Analog);
	}

	bool InputProcessorDualShock4::processDigital(USAGE identifier, bool pressed)
	{
		switch (identifier)
		{

		case 0x01:
			m_Bindings["Square"].digitalValue = pressed;
			break;

		case 0x02:
			m_Bindings["Cross"].digitalValue = pressed;
			break;

		case 0x03:
			m_Bindings["Circle"].digitalValue = pressed;
			break;

		case 0x04:
			m_Bindings["Triangle"].digitalValue = pressed;
			break;

		case 0x05:
			m_Bindings["L1"].digitalValue = pressed;
			break;

		case 0x06:
			m_Bindings["R1"].digitalValue = pressed;
			break;

		case 0x07:
			m_Bindings["L2"].digitalValue = pressed;
			break;

		case 0x08:
			m_Bindings["R2"].digitalValue = pressed;
			break;

		case 0x09:
			m_Bindings["Share"].digitalValue = pressed;
			break;

		case 0x0A:
			m_Bindings["Options"].digitalValue = pressed;
			break;

		case 0x0B:
			m_Bindings["LeftThumb"].digitalValue = pressed;
			break;

		case 0x0C:
			m_Bindings["RightThumb"].digitalValue = pressed;
			break;

		case 0x0D:
			m_Bindings["Guide"].digitalValue = pressed;
			break;

		default:
			break;

		}

		return true;
	}

	bool InputProcessorDualShock4::processAnalog(USAGE identifier, LONG value)
	{
		bool update_left_stick = false;
		bool update_right_stick = false;

		switch (identifier)
		{

		case 0x30:
			m_LeftStickValues[0] = value - 128;
			update_left_stick = true;
			break;

		case 0x31:
			m_LeftStickValues[1] = value - 128;
			update_left_stick = true;
			break;

		case 0x32:
			m_RightStickValues[0] = value - 128;
			update_right_stick = true;
			break;

		case 0x35:
			m_RightStickValues[1] = value - 128;
			update_right_stick = true;
			break;

		case 0x39:
			{
				// Directional pad

				m_Bindings["Up"].digitalValue = (value == 6 || value == 0 || value == 1);
				m_Bindings["Right"].digitalValue = (value == 1 || value == 2 || value == 3);
				m_Bindings["Down"].digitalValue = (value == 3 || value == 4 || value == 5);
				m_Bindings["Left"].digitalValue = (value == 5 || value == 6 || value == 7);
			}
			break;

		default:
			break;

		}

		static const float stick_dead_zone = 20.0f;
		static const LONG stick_dead_zone_squared = (LONG)(stick_dead_zone * stick_dead_zone);
		static const float stick_dead_zone_correction = ((float)UINT8_MAX / ((float)UINT8_MAX - stick_dead_zone)) / (((float)UINT8_MAX - stick_dead_zone) / 2.0f);

		if (update_left_stick)
		{
			LONG left_stick_length_squared = (m_LeftStickValues[0] * m_LeftStickValues[0]) + (m_LeftStickValues[1] * m_LeftStickValues[1]);
			if (left_stick_length_squared > stick_dead_zone_squared)
			{
				float x = (float)m_LeftStickValues[0];
				float y = (float)m_LeftStickValues[1];

				float length = sqrtf((float)left_stick_length_squared);
				float normalized_x = x / length;
				float normalized_y = y / length;

				m_Bindings["LeftStickHorizontal"].analogValue = (x - (normalized_x * stick_dead_zone)) * stick_dead_zone_correction;
				m_Bindings["LeftStickVertical"].analogValue = (y - (normalized_y * stick_dead_zone)) * stick_dead_zone_correction;
			}
			else
			{
				m_Bindings["LeftStickHorizontal"].analogValue = 0.0f;
				m_Bindings["LeftStickVertical"].analogValue = 0.0f;
			}
		}

		if (update_right_stick)
		{
			LONG right_stick_length_squared = (m_RightStickValues[0] * m_RightStickValues[0]) + (m_RightStickValues[1] * m_RightStickValues[1]);
			if (right_stick_length_squared > stick_dead_zone_squared)
			{
				float x = (float)m_RightStickValues[0];
				float y = (float)m_RightStickValues[1];

				float length = sqrtf((float)right_stick_length_squared);
				float normalized_x = x / length;
				float normalized_y = y / length;

				m_Bindings["RightStickHorizontal"].analogValue = (x - (normalized_x * stick_dead_zone)) * stick_dead_zone_correction;
				m_Bindings["RightStickVertical"].analogValue = (y - (normalized_y * stick_dead_zone)) * stick_dead_zone_correction;
			}
			else
			{
				m_Bindings["RightStickHorizontal"].analogValue = 0.0f;
				m_Bindings["RightStickVertical"].analogValue = 0.0f;
			}
		}

		return true;
	}

}; // namespace Pitstop