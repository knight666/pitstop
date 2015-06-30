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

		HANDLE getJoystickHandle() const;

		void setJoystick(RawInputJoystick& joystick);

		void setUsbDevice(UsbDevice& usb);

		void update();

	private:

		bool mapToXinput(XInputState& state, const QHash<QString, InputProcessorBase::InputBinding>& bindings);

	private:

		VirtualInputManager& m_Manager;
		uint8_t m_Index;
		RawInputJoystick* m_Joystick;
		UsbDevice* m_Usb;

	}; // class VirtualInputDevice

}; // namespace Pitstop