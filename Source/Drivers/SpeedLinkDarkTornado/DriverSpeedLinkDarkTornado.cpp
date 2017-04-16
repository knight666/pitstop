#include "DriverSpeedLinkDarkTornado.h"

namespace Pitstop {

	DriverSpeedLinkDarkTornado::DriverSpeedLinkDarkTornado()
	{
		strcpy_s(m_Info.name, "SpeedLink Dark Tornado");
		m_Info.product = 0x1A34;
		m_Info.vendor = 0x0805;

		addDigitalInput("Trigger1", 0x01);
		addDigitalInput("Trigger2", 0x02);
		addDigitalInput("Trigger3", 0x03);
		addDigitalInput("Trigger4", 0x04);
		addDigitalInput("Button5", 0x05);
		addDigitalInput("Button6", 0x06);
		addDigitalInput("Button7", 0x07);
		addDigitalInput("Button8", 0x08);

		addAnalogInput("StickHorizontal", 0x30, -127.0f, 127.0f);
		addAnalogInput("StickVertical", 0x31, -127.0f, 127.0f);
		addAnalogInput("Throttle", 0x36, 0.0f, 255.0f);
	}

	DriverSpeedLinkDarkTornado::~DriverSpeedLinkDarkTornado()
	{
	}

};