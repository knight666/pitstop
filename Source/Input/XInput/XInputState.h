#pragma once

#include "Base/Main.h"

namespace Pitstop {

	struct XInputState
	{
		enum class Button
		{
			Up,
			Down,
			Left,
			Right,
			Start,
			Back,
			LeftThumb,
			RightThumb,
			LeftBumper,
			RightBumper,
			Guide,
			Unknown,
			A,
			B,
			X,
			Y,

			_COUNT
		};

		enum class Axis
		{
			LeftTrigger,
			RightTrigger,
			LeftStickHorizontal,
			LeftStickVertical,
			RightStickHorizontal,
			RightStickVertical,

			_COUNT
		};

		uint8_t buttonState[Button::_COUNT];
		float axisState[Axis::_COUNT];

	}; // struct XInputState

}; // namespace Pitstop