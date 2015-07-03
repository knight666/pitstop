#pragma once

#include "Base/Main.h"
#include "Input/Process/InputProcessorBase.h"
#include "Input/Usb/UsbDevice.h"
#include "Input/RawInputJoystick.h"

namespace Pitstop {

	class VirtualInputManager;
	struct XInputState;

	class VirtualInputDevice
		: public QObject
	{

		Q_OBJECT

		friend class VirtualInputManager;

	public:

		VirtualInputDevice(VirtualInputManager& manager, uint8_t index);
		~VirtualInputDevice();

		HANDLE getJoystickHandle() const;

		void setJoystick(RawInputJoystickPtr joystick);

		void setUsbDevice(UsbDevicePtr usb);

	public slots:

		void slotJoystickInput(RawInputJoystick* joystick, bool processed);

	private:

		bool mapToXinput(XInputState& state, const QHash<QString, InputProcessorBase::InputBinding>& bindings);

	private:

		VirtualInputManager& m_Manager;
		uint8_t m_Index;
		RawInputJoystickPtr m_Joystick;
		UsbDevicePtr m_Usb;

	}; // class VirtualInputDevice

}; // namespace Pitstop