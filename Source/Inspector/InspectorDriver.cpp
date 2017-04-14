#include "InspectorDriver.h"

#include "Tracking/Tracking.h"

namespace Pitstop {

	InspectorDriver::InspectorDriver(Tracking& tracking)
		: m_Tracking(tracking)
	{
	}

	void InspectorDriver::createBindings()
	{
		m_Tracking.clear();
	}

	bool InspectorDriver::processDigital(USAGE identifier, bool pressed)
	{
		m_Tracking.trackValue(InputType::Digital, identifier, static_cast<LONG>(pressed));

		return true;
	}

	bool InspectorDriver::processAnalog(USAGE identifier, LONG value)
	{
		m_Tracking.trackValue(InputType::Analog, identifier, value);

		return true;
	}

};