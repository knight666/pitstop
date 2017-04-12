#include "InspectorDriver.h"

namespace Pitstop {

	InspectorDriver::InspectorDriver()
	{
	}

	void InspectorDriver::createBindings()
	{
		if (!m_Tracking.isEmpty())
		{
			m_Tracking.clear();
		}
	}

	bool InspectorDriver::processDigital(USAGE identifier, bool pressed)
	{
		trackValue(identifier, static_cast<LONG>(pressed));

		return true;
	}

	bool InspectorDriver::processAnalog(USAGE identifier, LONG value)
	{
		trackValue(identifier, value);

		return true;
	}

	void InspectorDriver::trackValue(USAGE identifier, LONG value)
	{
		QMap<LONG, int32_t>* tracked = nullptr;

		auto found = m_Tracking.find(identifier);
		if (found != m_Tracking.end())
		{
			tracked = &found.value();
		}
		else
		{
			m_Tracking.insert(identifier, QMap<LONG, int32_t>());
			tracked = &m_Tracking.find(identifier).value();
		}

		auto found_counter = tracked->find(value);
		if (found_counter == tracked->end())
		{
			tracked->insert(value, 0);
			found_counter = tracked->find(value);
		}

		found_counter.value() += 1;
	}

};