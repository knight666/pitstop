#pragma once

#include <Core/Main.h>
#include <Input/Types.h>

namespace Pitstop {

	class TrackingItem
	{

	public:

		TrackingItem(USAGE identifier, InputType type);

		const QString& getName() const { return m_name; }

		int32_t getValueLatest() const { return m_valueLatest; }
		int32_t getMinimum() const { return m_minimum; }
		int32_t getMaximum() const { return m_maximum; }
		float getAverage() const { return m_average; }

		void add(int32_t value);

		void serialize(QJsonObject& target, size_t version);

	public:

		USAGE m_identifier;
		InputType m_type;
		QString m_name;
		QMap<LONG, int32_t> m_values;
		int32_t m_valueLatest;
		float m_valueSamples;
		float m_valueTotal;
		int32_t m_minimum;
		int32_t m_maximum;
		float m_average;

	};

};