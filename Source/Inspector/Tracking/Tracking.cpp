#include "Tracking.h"

namespace Pitstop {

	void Tracking::clear()
	{
		m_Items.clear();
	}

	void Tracking::trackValue(InputType type, USAGE identifier, LONG value)
	{
		TrackingItem* tracked = nullptr;
		bool is_new = false;

		auto found = m_Items.find(identifier);
		if (found != m_Items.end())
		{
			tracked = &found.value();
		}
		else
		{
			m_Items.insert(identifier, TrackingItem());
			tracked = &m_Items.find(identifier).value();
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
			tracked->type = type;

			switch (type)
			{

			case InputType::Digital:
				tracked->name = QString("Digital%1").arg(identifier);
				break;

			case InputType::Analog:
				tracked->name = QString("Analog%1").arg(identifier);
				break;

			case InputType::Axis:
				tracked->name = QString("Axis%1").arg(identifier);
				break;

			default:
				break;

			}

			emit signalTrackingCreated(identifier, *tracked);
		}
	}

};