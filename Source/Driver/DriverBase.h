#pragma once

#include <Core/Main.h>
#include <Input/Types.h>

#include "DriverInfo.h"

namespace Pitstop {

	class RawInputJoystick;

	class DriverBase
	{

	public:

		DriverBase();
		virtual ~DriverBase();

		DriverInfo* getInfo() { return &m_Info; }

		bool setup(RawInputJoystick* joystick);
		bool process(const RAWINPUT& message);

	protected:

		void addDigitalInput(const char* name, USAGE identifier);
		void addAnalogInput(const char* name, USAGE identifier, float minimum, float maximum);

		virtual bool processDigital(USAGE identifier, bool value);
		virtual bool processAnalog(USAGE identifier, LONG value);

	protected:

		DriverInfo m_Info;
		QMap<USAGE, DigitalInput> m_DigitalInputs;
		QMap<USAGE, AnalogInput> m_AnalogInputs;

	private:

		RawInputJoystick* m_Joystick;

		QVector<uint8_t> m_PreparsedData;
		PHIDP_PREPARSED_DATA m_Preparsed;
		HIDP_CAPS m_Capabilities;
		QVector<HIDP_BUTTON_CAPS> m_ButtonCapabilities;
		QVector<HIDP_VALUE_CAPS> m_ValueCapabilities;
		QHash<USAGE, bool> m_ButtonsState;
		QHash<USAGE, LONG> m_AxisValues;

	};

};