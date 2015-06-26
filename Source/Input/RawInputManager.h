#pragma once

#include "Base/Main.h"

namespace Pitstop {

	class ProcessorBase;
	class RawInputJoystick;

	class RawInputManager
	{

	public:

		RawInputManager();
		~RawInputManager();

		bool initialize(HWND window);

		void processInputMessage(WPARAM wParam, LPARAM lParam);

	private:

		QHash<HANDLE, RawInputJoystick*> m_Joysticks;
		QHash<HANDLE, ProcessorBase*> m_Processors;

	}; // class RawInputManager

}; // namespace Pitstop