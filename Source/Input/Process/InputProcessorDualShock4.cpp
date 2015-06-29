#include "Input/Process/InputProcessorDualShock4.h"

namespace Pitstop {

	bool InputProcessorDualShock4::processDigital(USAGE identifier, bool pressed)
	{
		switch (identifier)
		{

		case 0x01:
			m_Bindings["Square"].digital = pressed;
			break;

		case 0x02:
			m_Bindings["Cross"].digital = pressed;
			break;

		case 0x03:
			m_Bindings["Circle"].digital = pressed;
			break;

		case 0x04:
			m_Bindings["Triangle"].digital = pressed;
			break;

		case 0x05:
			m_Bindings["L1"].digital = pressed;
			break;

		case 0x06:
			m_Bindings["R1"].digital = pressed;
			break;

		case 0x07:
			m_Bindings["L2"].digital = pressed;
			break;

		case 0x08:
			m_Bindings["R2"].digital = pressed;
			break;

		case 0x09:
			m_Bindings["Share"].digital = pressed;
			break;

		case 0x0A:
			m_Bindings["Options"].digital = pressed;
			break;

		case 0x0B:
			m_Bindings["LeftThumb"].digital = pressed;
			break;

		case 0x0C:
			m_Bindings["RightThumb"].digital = pressed;
			break;

		case 0x0D:
			m_Bindings["Guide"].digital = pressed;
			break;

		default:
			break;

		}

		return true;
	}

	bool InputProcessorDualShock4::processAnalog(USAGE identifier, LONG value)
	{
		return true;
	}

}; // namespace Pitstop