#pragma once

#include <Core/Main.h>

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

		uint8_t buttonState[static_cast<size_t>(Button::_COUNT)] = { 0 };
		float axisState[static_cast<size_t>(Axis::_COUNT)] = { 0 };

	}; // struct XInputState

}; // namespace Pitstop