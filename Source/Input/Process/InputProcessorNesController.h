#pragma once

#include <Core/Main.h>
#include "Input/Process/InputProcessorBase.h"

namespace Pitstop {

	class InputProcessorNesController
		: public InputProcessorBase
	{

	public:
		IMPLEMENT_INPUT_PROCESSOR(InputProcessorNesController, 0x0810, 0xe501);

	public:
		// InputProcessorBase
		virtual void createBindings() override;
		virtual bool processDigital(USAGE identifier, bool pressed) override;
		virtual bool processAnalog(USAGE identifier, LONG value) override;
		// ~InputProcessorBase

	};

}; // namespace Pitstop