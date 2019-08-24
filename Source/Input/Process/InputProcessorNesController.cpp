#include "Input/Process/InputProcessorNesController.h"

namespace Pitstop {

	void InputProcessorNesController::createBindings()
	{
		addBinding("Up", InputType::Digital);
		addBinding("Right", InputType::Digital);
		addBinding("Down", InputType::Digital);
		addBinding("Left", InputType::Digital);
		addBinding("Select", InputType::Digital);
		addBinding("Start", InputType::Digital);
		addBinding("B", InputType::Digital);
		addBinding("A", InputType::Digital);
	}

	bool InputProcessorNesController::processDigital(USAGE identifier, bool pressed)
	{
		switch (identifier)
		{

		case 0x01:
			getBinding("B").digitalValue = pressed;
			break;

		case 0x02:
			getBinding("A").digitalValue = pressed;
			break;

		case 0x09:
			getBinding("Select").digitalValue = pressed;
			break;

		case 0x0A:
			getBinding("Start").digitalValue = pressed;
			break;

		default:
			break;

		}

		return true;
	}

	bool InputProcessorNesController::processAnalog(USAGE identifier, LONG value)
	{
		switch (identifier)
		{

		case 48:
			if (value < 127)
			{
				getBinding("Left").digitalValue = true;
				getBinding("Right").digitalValue = false;
			}
			else
			{
				getBinding("Left").digitalValue = false;
				getBinding("Right").digitalValue = (value != 127);
			}
			break;

		case 49:
			if (value < 127)
			{
				getBinding("Up").digitalValue = true;
				getBinding("Down").digitalValue = false;
			}
			else
			{
				getBinding("Up").digitalValue = false;
				getBinding("Down").digitalValue = (value != 127);
			}
			break;

		default:
			break;

		}

		return true;
	}

};