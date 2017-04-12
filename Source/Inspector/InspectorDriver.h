#pragma once

#include <Core/Main.h>
#include <Input/Process/InputProcessorBase.h>

namespace Pitstop {

	class InspectorDriver
		: public InputProcessorBase
	{

	private:

		virtual void createBindings() override;

		virtual bool processDigital(USAGE identifier, bool pressed) override;
		virtual bool processAnalog(USAGE identifier, LONG value) override;

	};

};