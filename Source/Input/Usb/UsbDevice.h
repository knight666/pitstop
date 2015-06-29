#pragma once

#include "Base/Main.h"

namespace Pitstop {

	class UsbController;

	class UsbDevice
	{

	public:

		UsbDevice(UsbController& controller, size_t identifier);
		~UsbDevice();

	private:

		UsbController& m_Controller;
		size_t m_Identifier;

	}; // class UsbDevice

}; // namespace Pitstop