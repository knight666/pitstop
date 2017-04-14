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

	void Tracking::serialize(QJsonObject& target, size_t version)
	{
		QJsonArray bindings;
		for (auto& it : m_Items)
		{
			QJsonObject item;
			it.serialize(item, version);
			bindings.append(item);
		}
		target["bindings"] = bindings;
	}

};