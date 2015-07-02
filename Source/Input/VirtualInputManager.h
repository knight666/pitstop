#pragma once

#include "Base/Main.h"

#include "Input/VirtualInputDevice.h"

namespace Pitstop {

	class RawInputJoystick;
	class UsbDevice;

	class VirtualInputManager
		: public QObject
	{

		Q_OBJECT

	public:

		VirtualInputManager();
		~VirtualInputManager();

		VirtualInputDevice* getDeviceByIndex(uint8_t index) const;
		VirtualInputDevice* getDeviceByHandle(HANDLE handle) const;

		void update(HANDLE handle);

	public slots:

		void slotJoystickInput(RawInputJoystick* joystick);

	private:

		QVector<VirtualInputDevice*> m_Devices;
		QHash<HANDLE, VirtualInputDevice*> m_RawInputMapping;

	}; // class VirtualInputManager

}; // namespace Pitstop