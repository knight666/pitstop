#pragma once

#include <Core/Main.h>
#include "Input/Process/InputProcessorBase.h"

namespace Pitstop {

	class InputProcessorFFBWheel
		: public InputProcessorBase
	{

	public:

		IMPLEMENT_INPUT_PROCESSOR(InputProcessorFFBWheel, 0x06F8, 0x0004);

	private:

		virtual void createBindings() override;

		virtual bool processDigital(USAGE identifier, bool pressed) override;
		virtual bool processAnalog(USAGE identifier, LONG value) override;

	}; // class InputProcessorFFBWheel

}; // namespace Pitstop