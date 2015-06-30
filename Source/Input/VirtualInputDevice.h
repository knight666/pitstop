#pragma once

#include "Base/Main.h"
#include "Input/Process/InputProcessorBase.h"

namespace Pitstop {

	class RawInputJoystick;
	class UsbDevice;
	class VirtualInputManager;
	struct XInputState;

	class VirtualInputDevice
	{

		friend class VirtualInputManager;

	public:

		VirtualInputDevice(VirtualInputManager& manager, uint8_t index);
		~VirtualInputDevice();

		void update();

	private:

		bool mapToXinput(XInputState& state, const QHash<QString, InputProcessorBase::InputBinding>& bindings);

		void setJoystick(RawInputJoystick& joystick);

		void setUsbDevice(UsbDevice& usb);

	private:

		VirtualInputManager& m_Manager;
		uint8_t m_Index;
		RawInputJoystick* m_Joystick;
		UsbDevice* m_Usb;

	}; // class VirtualInputDevice

}; // namespace Pitstop