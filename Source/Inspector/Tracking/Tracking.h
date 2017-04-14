#pragma once

#include <Core/Main.h>
#include <Input/Types.h>

#include "TrackingItem.h"

namespace Pitstop {

	class Tracking
		: public QObject
	{

		Q_OBJECT

	public:

		void clear();
		void trackValue(InputType type, USAGE identifier, LONG value);

	signals:

		void signalTrackingCreated(USAGE identifier, TrackingItem& item);

	private:

		QMap<USAGE, TrackingItem> m_Items;

	};

};