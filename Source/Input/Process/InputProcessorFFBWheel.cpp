#include "Input/Process/InputProcessorFFBWheel.h"

namespace Pitstop {

	void InputProcessorFFBWheel::createBindings()
	{
		addBinding("UpTrigger", InputType::Digital);
		addBinding("DownTrigger", InputType::Digital);
		addBinding("RightThumbButton", InputType::Digital);
		addBinding("LeftThumbButton", InputType::Digital);
		addBinding("GearBoxUp", InputType::Digital);
		addBinding("GearBoxDown", InputType::Digital);
		addBinding("DirectionalPadUp", InputType::Digital);
		addBinding("DirectionalPadRight", InputType::Digital);
		addBinding("DirectionalPadDown", InputType::Digital);
		addBinding("DirectionalPadLeft", InputType::Digital);
	}

	bool InputProcessorFFBWheel::processDigital(USAGE identifier, bool pressed)
	{
		switch (identifier)
		{

		case 0x01:
			m_Bindings["UpTrigger"].digitalValue = pressed;
			break;

		case 0x02:
			m_Bindings["DownTrigger"].digitalValue = pressed;
			break;

		case 0x03:
			m_Bindings["RightThumbButton"].digitalValue = pressed;
			break;

		case 0x04:
			m_Bindings["LeftThumbButton"].digitalValue = pressed;
			break;

		case 0x05:
			m_Bindings["GearBoxUp"].digitalValue = pressed;
			break;

		case 0x06:
			m_Bindings["GearBoxDown"].digitalValue = pressed;
			break;

		case 0x07:
			m_Bindings["DirectionalPadUp"].digitalValue = pressed;
			break;

		case 0x08:
			m_Bindings["DirectionalPadRight"].digitalValue = pressed;
			break;

		case 0x09:
			m_Bindings["DirectionalPadDown"].digitalValue = pressed;
			break;

		case 0x0A:
			m_Bindings["DirectionalPadLeft"].digitalValue = pressed;
			break;

		default:
			break;
		}

		return true;
	}

	bool InputProcessorFFBWheel::processAnalog(USAGE identifier, LONG value)
	{
		return true;
	}

}; // namespace Pitstop