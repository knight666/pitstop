#include "Input/Usb/UsbDevice.h"

namespace Pitstop {

	UsbDevice::UsbDevice(UsbController& controller, size_t identifier)
		: m_Controller(controller)
		, m_Identifier(identifier)
	{
	}

	UsbDevice::~UsbDevice()
	{
	}

}; // namespace Pitstop