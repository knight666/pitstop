#pragma once

#include <Core/Main.h>
#include <Input/Process/InputProcessorBase.h>
#include <Input/Types.h>

namespace Pitstop {

	class Tracking;

	class InspectorDriver
		: public QObject
		, public InputProcessorBase
	{

		Q_OBJECT

	public:

		InspectorDriver(Tracking& tracking);

	private:

		virtual void createBindings() override;

		virtual bool processDigital(USAGE identifier, bool pressed) override;
		virtual bool processAnalog(USAGE identifier, LONG value) override;

	private:

		Tracking& m_Tracking;

	};

};