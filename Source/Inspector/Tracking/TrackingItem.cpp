#include "TrackingItem.h"

namespace Pitstop {

	TrackingItem::TrackingItem(USAGE identifier, InputType type)
		: m_identifier(identifier)
		, m_type(type)
		, m_valueSamples(0.0f)
		, m_valueTotal(0.0f)
		, m_minimum(std::numeric_limits<int32_t>::max())
		, m_maximum(std::numeric_limits<int32_t>::min())
		, m_average(0.0f)
	{
		switch (type)
		{

		case InputType::Digital:
			m_name = QString("Digital%1").arg(identifier);
			break;

		case InputType::Analog:
			m_name = QString("Analog%1").arg(identifier);
			break;

		case InputType::Axis:
			m_name = QString("Axis%1").arg(identifier);
			break;

		default:
			break;

		}
	}

	void TrackingItem::add(int32_t value)
	{
		m_valueLatest = value;

		auto found = m_values.find(value);
		if (found == m_values.end())
		{
			m_values.insert(value, 0);
			found = m_values.find(value);
		}

		found.value() += 1;

		m_valueSamples += 1.0f;
		m_valueTotal += static_cast<float>(value);

		m_minimum = std::min(m_minimum, value);
		m_maximum = std::max(m_maximum, value);
		m_average = m_valueTotal / m_valueSamples;
	}

	void TrackingItem::serialize(QJsonArray& bindings)
	{
		QJsonObject item;
		item["identifier"] = m_identifier;
		item["name"] = m_name;

		QJsonObject samples;
		for (auto it = m_values.begin(); it != m_values.end(); ++it)
		{
			samples[QString("%1").arg(it.key(), 4)] = it.value();
		}
		item["samples"] = samples;

		bindings.append(item);
	}

};