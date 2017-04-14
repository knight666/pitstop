#include "Tracking.h"

namespace Pitstop {

	void Tracking::clear()
	{
		m_Items.clear();
	}

	void Tracking::trackValue(InputType type, USAGE identifier, LONG value)
	{
		TrackingItem* tracked = nullptr;

		auto found = m_Items.find(identifier);
		if (found != m_Items.end())
		{
			tracked = &found.value();
		}
		else
		{
			m_Items.insert(identifier, TrackingItem(identifier, type));
			tracked = &m_Items.find(identifier).value();
		}

		tracked->add(value);

		emit signalTrackingUpdated(identifier, *tracked);
	}

};