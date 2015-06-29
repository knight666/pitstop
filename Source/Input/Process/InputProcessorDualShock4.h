#pragma once

#include "Base/Main.h"
#include "Input/Process/InputProcessorBase.h"

namespace Pitstop {

	class InputProcessorDualShock4
		: public InputProcessorBase
	{

	public:

		IMPLEMENT_INPUT_PROCESSOR(InputProcessorDualShock4, 0x054C, 0x05C4);

	private:

		virtual void createBindings() override;

		virtual bool processDigital(USAGE identifier, bool pressed) override;
		virtual bool processAnalog(USAGE identifier, LONG value) override;

	}; // class InputProcessorDualShock4

}; // namespace Pitstop