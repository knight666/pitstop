#pragma once

#include <Core/Main.h>
#include <Input/Types.h>

namespace Pitstop {

	class TrackingItem
	{

	public:

		TrackingItem(USAGE identifier, InputType type);

		void add(int32_t value);

		const QString& getName() const { return name; }

		int32_t getMinimum() const { return minimum; }
		int32_t getMaximum() const { return maximum; }
		float getAverage() const { return average; }
		float getMedian() const { return median; }

	public:

		InputType type;
		QString name;
		QMap<LONG, int32_t> values;
		int32_t minimum;
		int32_t maximum;
		float average;
		float median;

	};

};