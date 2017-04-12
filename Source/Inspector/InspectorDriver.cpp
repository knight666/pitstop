#include "InspectorDriver.h"

namespace Pitstop {

	void InspectorDriver::createBindings()
	{

	}

	bool InspectorDriver::processDigital(USAGE identifier, bool pressed)
	{
		return true;
	}

	bool InspectorDriver::processAnalog(USAGE identifier, LONG value)
	{
		return true;
	}

};