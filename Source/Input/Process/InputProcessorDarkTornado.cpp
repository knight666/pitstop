#include "Input/Process/InputProcessorDarkTornado.h"

namespace Pitstop {

	void InputProcessorDarkTornado::createBindings()
	{
		addBinding("Trigger1", InputType::Digital);
		addBinding("Trigger2", InputType::Digital);
		addBinding("Trigger3", InputType::Digital);
		addBinding("Trigger4", InputType::Digital);
		addBinding("Button5", InputType::Digital);
		addBinding("Button6", InputType::Digital);
		addBinding("Button7", InputType::Digital);
		addBinding("Button8", InputType::Digital);

		addBinding("StickHorizontal", InputType::Analog);
		addBinding("StickVertical", InputType::Analog);
		addBinding("Throttle", InputType::Analog);
	}

	bool InputProcessorDarkTornado::processDigital(USAGE identifier, bool pressed)
	{
		switch (identifier)
		{

		case 0x01:
			m_Bindings["Trigger1"].digitalValue = pressed;
			break;

		case 0x02:
			m_Bindings["Trigger1"].digitalValue = pressed;
			break;

		case 0x03:
			m_Bindings["Trigger3"].digitalValue = pressed;
			break;

		case 0x04:
			m_Bindings["Trigger4"].digitalValue = pressed;
			break;

		case 0x05:
			m_Bindings["Button5"].digitalValue = pressed;
			break;

		case 0x06:
			m_Bindings["Button6"].digitalValue = pressed;
			break;

		case 0x07:
			m_Bindings["Button7"].digitalValue = pressed;
			break;

		case 0x08:
			m_Bindings["Button8"].digitalValue = pressed;
			break;

		default:
			break;

		}

		return true;
	}

	bool InputProcessorDarkTornado::processAnalog(USAGE identifier, LONG value)
	{
		switch (identifier)
		{

		case 0x30:
			m_Bindings["StickHorizontal"].analogValue = static_cast<float>(value) / 127.0f;
			break;

		case 0x31:
			m_Bindings["StickVertical"].analogValue = static_cast<float>(value) / 127.0f;
			break;

		case 0x36:
			m_Bindings["Throttle"].analogValue = static_cast<float>(value) / 255.0f;
			break;

		default:
			break;

		}

		return true;
	}

};