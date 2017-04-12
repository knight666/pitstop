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
		TrackingItem* tracked = nullptr;
		bool is_new = false;

		auto found = m_Tracking.find(identifier);
		if (found != m_Tracking.end())
		{
			tracked = &found.value();
		}
		else
		{
			m_Tracking.insert(identifier, TrackingItem());
			tracked = &m_Tracking.find(identifier).value();
			is_new = true;
		}

		auto found_counter = tracked->values.find(value);
		if (found_counter == tracked->values.end())
		{
			tracked->values.insert(value, 0);
			found_counter = tracked->values.find(value);
		}

		found_counter.value() += 1;

		if (is_new)
		{
			tracked->name = QString("Input%1").arg(identifier);
			emit signalTrackingCreated(identifier, *tracked);
		}
	}

};