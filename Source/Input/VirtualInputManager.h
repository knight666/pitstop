#pragma once

#include "Base/Main.h"

#include "Input/VirtualInputDevice.h"

namespace Pitstop {

	class RawInputJoystick;
	class RawInputManager;
	class UsbDevice;

	class VirtualInputManager
		: public QObject
	{

		Q_OBJECT

	public:

		VirtualInputManager(RawInputManager& rawInput);
		~VirtualInputManager();

		VirtualInputDevice* getDeviceByIndex(uint8_t index) const;
		VirtualInputDevice* getDeviceByHandle(HANDLE handle) const;

	public slots:

		void slotJoystickInput(RawInputJoystick* joystick);

	private:

		RawInputManager& m_RawInput;
		QVector<VirtualInputDevice*> m_Devices;
		QHash<HANDLE, VirtualInputDevice*> m_RawInputMapping;

	}; // class VirtualInputManager

}; // namespace Pitstop