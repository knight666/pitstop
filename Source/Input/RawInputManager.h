#pragma once

#include "Base/Main.h"

namespace Pitstop {

	class RawInputJoystick;

	class RawInputManager
	{

	public:

		RawInputManager();
		~RawInputManager();

		bool initialize();

		void processInputMessage(WPARAM wParam, LPARAM lParam);

	private:

		QHash<HANDLE, RawInputJoystick*> m_Joysticks;

	}; // class RawInputManager

}; // namespace Pitstop