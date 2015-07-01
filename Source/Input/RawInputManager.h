#pragma once

#include "Base/Main.h"
#include "Input/Process/InputProcessorBase.h"

namespace Pitstop {

	class RawInputJoystick;

	class RawInputManager
	{

	public:

		RawInputManager();
		~RawInputManager();

		bool initialize(HWND window);

		void setDeviceConnected(HANDLE device, bool connected);

		// TEMP
		RawInputJoystick* getJoystick() const;

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

	private:

		void registerInputProcessor(uint16_t vendor, uint16_t product, InputProcessorBase::FactoryMethod method);

	private:

		QHash<HANDLE, RawInputJoystick*> m_Joysticks;
		QHash<uint32_t, std::function<InputProcessorBase::FactoryMethod>> m_InputProcessorFactories;

	}; // class RawInputManager

}; // namespace Pitstop