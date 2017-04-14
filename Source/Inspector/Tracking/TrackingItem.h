#pragma once

#include <Core/Main.h>
#include <Input/Types.h>

namespace Pitstop {

	struct TrackingItem
	{
		InputType type;
		QString name;
		QMap<LONG, int32_t> values;
	};

};