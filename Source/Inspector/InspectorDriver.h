#pragma once

#include <Core/Main.h>
#include <Input/Process/InputProcessorBase.h>

namespace Pitstop {

	struct TrackingItem
	{
		QString name;
		QMap<LONG, int32_t> values;
	};

	class InspectorDriver
		: public QObject
		, public InputProcessorBase
	{

		Q_OBJECT

	public:

		InspectorDriver();

	signals:

		void signalTrackingCreated(USAGE identifier, TrackingItem& item);

	private:

		virtual void createBindings() override;

		virtual bool processDigital(USAGE identifier, bool pressed) override;
		virtual bool processAnalog(USAGE identifier, LONG value) override;

		void trackValue(USAGE identifier, LONG value);

	private:

		QMap<USAGE, TrackingItem> m_Tracking;

	};

};