#include "Input/Process/InputProcessorDualShock4.h"

namespace Pitstop {

	bool InputProcessorDualShock4::processDigital(USAGE identifier, bool pressed)
	{
		return true;
	}

	bool InputProcessorDualShock4::processAnalog(USAGE identifier, LONG value)
	{
		return true;
	}

}; // namespace Pitstop