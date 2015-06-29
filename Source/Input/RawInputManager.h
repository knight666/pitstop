#pragma once

#include "Base/Main.h"

namespace Pitstop {

	class RawInputJoystick;
	class XInputDevice;

	class RawInputManager
	{

	public:

		RawInputManager();
		~RawInputManager();

		bool initialize(HWND window);

		void processInputMessage(WPARAM wParam, LPARAM lParam);

	private:

		QHash<HANDLE, RawInputJoystick*> m_Joysticks;
		QHash<HANDLE, XInputDevice*> m_XInputDevices;
		size_t m_NextXInputIdentifier;

	}; // class RawInputManager

}; // namespace Pitstop