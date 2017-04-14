#include "TrackingItem.h"

namespace Pitstop {

	TrackingItem::TrackingItem(USAGE identifier, InputType type)
		: type(type)
		, minimum(std::numeric_limits<int32_t>::max())
		, maximum(std::numeric_limits<int32_t>::min())
		, average(0.0f)
		, median(0.0f)
	{
		switch (type)
		{

		case InputType::Digital:
			name = QString("Digital%1").arg(identifier);
			break;

		case InputType::Analog:
			name = QString("Analog%1").arg(identifier);
			break;

		case InputType::Axis:
			name = QString("Axis%1").arg(identifier);
			break;

		default:
			break;

		}
	}

	void TrackingItem::add(int32_t value)
	{
		auto found = values.find(value);
		if (found == values.end())
		{
			values.insert(value, 0);
			found = values.find(value);
		}

		found.value() += 1;

		minimum = std::min(minimum, value);
		maximum = std::max(maximum, value);
	}

};