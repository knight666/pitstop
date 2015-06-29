#pragma once

#include "Base/Main.h"
#include "Input/Process/InputProcessorBase.h"

namespace Pitstop {

	class RawInputJoystick;
	class XInputDevice;

	class RawInputManager
	{

	public:

		RawInputManager();
		~RawInputManager();

		bool initialize(HWND window);

		InputProcessorBase* createInputProcessor(RawInputJoystick& joystick);
		void registerInputProcessor(InputProcessorBase::FactoryMethod method);

		void processInputMessage(WPARAM wParam, LPARAM lParam);

	private:

		QHash<HANDLE, RawInputJoystick*> m_Joysticks;
		QHash<HANDLE, XInputDevice*> m_XInputDevices;
		size_t m_NextXInputIdentifier;
		QVector<std::function<InputProcessorBase::FactoryMethod>> m_InputProcessorFactories;

	}; // class RawInputManager

}; // namespace Pitstop