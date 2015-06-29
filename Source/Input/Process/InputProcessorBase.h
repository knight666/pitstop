#pragma once

#include "Base/Main.h"
#include "Input/RawInputJoystick.h"

namespace Pitstop {

#define IMPLEMENT_INPUT_PROCESSOR(_type, _vid, _pid) \
	static InputProcessorBase* create(RawInputJoystick& joystick) { \
		return joystick.matchVendorAndProduct(_vid, _pid) ? new _type(joystick) : nullptr; \
	} \
	_type(RawInputJoystick& joystick) : InputProcessorBase(joystick) { }

	class InputProcessorBase
	{

	public:

		enum InputState
		{
			InputState_Up = 0,
			InputState_Pressed = 0x01,
			InputState_Down = 0x02,
			InputState_Released = 0x04,
		};

		typedef InputProcessorBase* (FactoryMethod(RawInputJoystick&));

		InputProcessorBase(RawInputJoystick& joystick);
		virtual ~InputProcessorBase();

		virtual bool setup();

		bool process(const RAWINPUT& message);

	protected:

		virtual bool processDigital(USAGE identifier, bool pressed);
		virtual bool processAnalog(USAGE identifier, LONG value);

	protected:

		RawInputJoystick& m_Joystick;
		QVector<uint8_t> m_PreparsedData;
		PHIDP_PREPARSED_DATA m_Preparsed;
		HIDP_CAPS m_Capabilities;
		QVector<HIDP_BUTTON_CAPS> m_ButtonCapabilities;
		QVector<HIDP_VALUE_CAPS> m_ValueCapabilities;
		QHash<USAGE, bool> m_ButtonsState;
		QHash<USAGE, LONG> m_AxisValues;

	}; // class ProcessorBase

}; // namespace Pitstop