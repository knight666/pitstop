#pragma once

#include "Base/Main.h"

namespace Pitstop {

	class RawInputJoystick;

	class ProcessorBase
	{

	public:

		enum InputState
		{
			InputState_Up = 0,
			InputState_Pressed = 0x01,
			InputState_Down = 0x02,
			InputState_Released = 0x04,
		};

		ProcessorBase(RawInputJoystick& joystick);
		virtual ~ProcessorBase();

		virtual bool setup();

		bool process(const RAWINPUT& message);

	protected:

		virtual bool processButtonState(size_t index, uint8_t state);
		virtual bool processAxisState(size_t index, LONG value);

	protected:

		RawInputJoystick& m_Joystick;
		QVector<uint8_t> m_PreparsedData;
		PHIDP_PREPARSED_DATA m_Preparsed;
		HIDP_CAPS m_Capabilities;
		QVector<HIDP_BUTTON_CAPS> m_ButtonCapabilities;
		QVector<HIDP_VALUE_CAPS> m_ValueCapabilities;
		QVector<uint8_t> m_ButtonState;
		QHash<USAGE, LONG> m_AxisValues;

	}; // class ProcessorBase

}; // namespace Pitstop