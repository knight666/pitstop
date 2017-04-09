#pragma once

#include <Core/Main.h>
#include "Input/Process/InputProcessorBase.h"

namespace Pitstop {

	class InputProcessorDarkTornado
		: public InputProcessorBase
	{

	public:
		IMPLEMENT_INPUT_PROCESSOR(InputProcessorDarkTornado, 0x1A34, 0x0805);

	public:
		virtual void createBindings() override;

		virtual bool processDigital(USAGE identifier, bool pressed) override;
		virtual bool processAnalog(USAGE identifier, LONG value) override;

	};

}; // namespace Pitstop