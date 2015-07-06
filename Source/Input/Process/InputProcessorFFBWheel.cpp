#include "Input/Process/InputProcessorFFBWheel.h"

namespace Pitstop {

	void InputProcessorFFBWheel::createBindings()
	{
		addBinding("LeftGearShiftDown", InputType::Digital);
		addBinding("RightGearShiftUp", InputType::Digital);
		addBinding("LeftStickUp", InputType::Digital);
		addBinding("LeftStickRight", InputType::Digital);
		addBinding("LeftStickDown", InputType::Digital);
		addBinding("LeftStickLeft", InputType::Digital);
		addBinding("RightStickUp", InputType::Digital);
		addBinding("RightStickRight", InputType::Digital);
		addBinding("RightStickDown", InputType::Digital);
		addBinding("RightStickLeft", InputType::Digital);
		addBinding("LeftThumbButton", InputType::Digital);
		addBinding("RightThumbButton", InputType::Digital);
		addBinding("GearShiftDown", InputType::Digital);
		addBinding("GearShiftUp", InputType::Digital);

		addBinding("Wheel", InputType::Analog);
		addBinding("LeftFootPaddle", InputType::Analog);
		addBinding("RightFootPaddle", InputType::Analog);
	}

	bool InputProcessorFFBWheel::processDigital(USAGE identifier, bool pressed)
	{
		switch (identifier)
		{

		case 0x01:
			m_Bindings["RightGearShiftUp"].digitalValue = pressed;
			break;

		case 0x02:
			m_Bindings["LeftGearShiftDown"].digitalValue = pressed;
			break;

		case 0x03:
			m_Bindings["RightThumbButton"].digitalValue = pressed;
			break;

		case 0x04:
			m_Bindings["LeftThumbButton"].digitalValue = pressed;
			break;

		case 0x05:
			m_Bindings["GearShiftUp"].digitalValue = pressed;
			break;

		case 0x06:
			m_Bindings["GearShiftDown"].digitalValue = pressed;
			break;

		case 0x07:
			m_Bindings["RightStickUp"].digitalValue = pressed;
			break;

		case 0x08:
			m_Bindings["RightStickRight"].digitalValue = pressed;
			break;

		case 0x09:
			m_Bindings["RightStickDown"].digitalValue = pressed;
			break;

		case 0x0A:
			m_Bindings["RightStickLeft"].digitalValue = pressed;
			break;

		default:
			break;
		}

		return true;
	}

	bool InputProcessorFFBWheel::processAnalog(USAGE identifier, LONG value)
	{
		switch (identifier)
		{

		case 0x30:
			{
				float normalized = ((float)value / 2048.0f) - 1.0f;
				float magnitude = fabs(normalized);

				static const float dead_zone = 512.0f / 4096.0f;
				if (magnitude < dead_zone)
				{
					m_Bindings["Wheel"].analogValue = 0.0f;
				}
				else
				{
					m_Bindings["Wheel"].analogValue = normalized * ((magnitude - dead_zone) / (1.0f - dead_zone));
				}
			}
			break;

		case 0x31:
			m_Bindings["LeftFootPaddle"].analogValue = 1.0f - ((float)value / 510.0f);
			break;

		case 0x32:
			m_Bindings["RightFootPaddle"].analogValue = 1.0f - ((float)value / 255.0f);
			break;

		case 0x39:
			m_Bindings["LeftStickUp"].digitalValue = (value == 7 || value == 0 || value == 1);
			m_Bindings["LeftStickRight"].digitalValue = (value == 1 || value == 2 || value == 3);
			m_Bindings["LeftStickDown"].digitalValue = (value == 3 || value == 4 || value == 5);
			m_Bindings["LeftStickLeft"].digitalValue = (value == 5 || value == 6 || value == 7);
			break;

		default:
			break;

		}

		return true;
	}

}; // namespace Pitstop