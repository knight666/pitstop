#include "Input/Process/InputProcessorFFBWheel.h"

namespace Pitstop {

	void InputProcessorFFBWheel::createBindings()
	{

	}

	bool InputProcessorFFBWheel::processDigital(USAGE identifier, bool pressed)
	{
		return true;
	}

	bool InputProcessorFFBWheel::processAnalog(USAGE identifier, LONG value)
	{
		return true;
	}

}; // namespace Pitstop