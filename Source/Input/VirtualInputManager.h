#pragma once

#include "Base/Main.h"

#include "Input/VirtualInputDevice.h"

namespace Pitstop {

	class RawInputJoystick;
	class UsbDevice;

	class VirtualInputManager
	{

	public:

		VirtualInputManager();
		~VirtualInputManager();

		void bindJoystick(uint8_t index, RawInputJoystick& joystick);

		void bindUsbDevice(uint8_t index, UsbDevice& usb);

		void update(HANDLE device);

	private:

		QVector<VirtualInputDevice*> m_Devices;
		QHash<HANDLE, VirtualInputDevice*> m_RawInputMapping;

	}; // class VirtualInputManager

}; // namespace Pitstop