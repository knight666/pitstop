#pragma once

#include "Base/Main.h"
#include "Input/Process/InputProcessorBase.h"

namespace Pitstop {

	class RawInputJoystick;

	class RawInputManager
		: public QObject
	{

		Q_OBJECT

	public:

		RawInputManager();
		~RawInputManager();

		bool initialize(HWND window);

		void processInput(LPARAM lParam, WPARAM wParam);
		void processConnectionChanged(LPARAM lParam, WPARAM wParam);

		// TEMP
		RawInputJoystickPtr getJoystick() const;
		RawInputJoystickPtr getJoystickByHandle(HANDLE device) const;

		InputProcessorBase* createInputProcessor(RawInputJoystick& joystick);

		template <typename InputType>
		void registerInputProcessor()
		{
			registerInputProcessor(
				InputType::Vendor,
				InputType::Product,
				InputType::create);
		}

		void processInputMessage(const RAWINPUT& message, HANDLE device);

	signals:

		void signalJoystickInput(RawInputJoystickPtr joystick);

	private:

		RawInputJoystickPtr createJoystick(HANDLE device);
		void registerInputProcessor(uint16_t vendor, uint16_t product, InputProcessorBase::FactoryMethod method);

	private:

		bool m_Initialized;
		HWND m_Window;
		QHash<HANDLE, RawInputJoystickPtr> m_JoysticksByHandle;
		QHash<QString, RawInputJoystickPtr> m_JoysticksByGuid;
		QHash<uint32_t, std::function<InputProcessorBase::FactoryMethod>> m_InputProcessorFactories;

	}; // class RawInputManager

}; // namespace Pitstop