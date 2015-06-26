#pragma once

#include "Base/Main.h"

namespace Pitstop {

	class RawInputJoystick;

	class ProcessorBase
	{

	public:

		ProcessorBase(RawInputJoystick& joystick);
		virtual ~ProcessorBase();

		virtual bool setup();

	protected:

		RawInputJoystick& m_Joystick;
		uint8_t* m_PreparsedData;
		PHIDP_PREPARSED_DATA m_Preparsed;
		HIDP_CAPS m_Capabilities;
		QVector<HIDP_BUTTON_CAPS> m_ButtonCapabilities;
		QVector<HIDP_VALUE_CAPS> m_ValueCapabilities;

	}; // class ProcessorBase

}; // namespace Pitstop