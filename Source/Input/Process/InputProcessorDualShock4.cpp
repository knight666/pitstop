#include "Input/Process/InputProcessorDualShock4.h"

namespace Pitstop {

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
		return true;
	}

	void InputProcessorDualShock4::createBindings()
	{
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
	}

}; // namespace Pitstop