#pragma once

#include "Base/Main.h"

namespace Pitstop {

	class RawInputJoystick;

	class XInputDevice
	{

	public:

		XInputDevice(size_t controllerIndex);
		~XInputDevice();

		bool isActive() const { return m_DeviceHandle != NULL; }

		bool attach(RawInputJoystick& joystick);
		void detach();

		bool isPluggedIn() const { return m_PluggedIn; }
		void setPluggedIn(bool value);

		void writeOutput();

	private:

		size_t m_ControllerIndex;
		RawInputJoystick* m_Joystick;
		bool m_PluggedIn;
		HANDLE m_DeviceHandle;

	}; // class XInputDevice

}; // namespace Pitstop