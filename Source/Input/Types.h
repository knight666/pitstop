#pragma once

namespace Pitstop {

	enum class InputType
	{
		Digital,
		Analog,
		Axis
	};

	enum InputState
	{
		InputState_Up = 0,
		InputState_Pressed = 0x01,
		InputState_Down = 0x02,
		InputState_Released = 0x04,
	};

};