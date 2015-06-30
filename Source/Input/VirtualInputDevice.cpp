#include "Input/VirtualInputDevice.h"

#include "Input/RawInputJoystick.h"
#include "INput/Usb/UsbDevice.h"
#include "Input/VirtualInputManager.h"
#include "Input/XInputState.h"

namespace Pitstop {

	VirtualInputDevice::VirtualInputDevice(VirtualInputManager& manager, uint8_t index)
		: m_Manager(manager)
		, m_Index(index)
		, m_Joystick(nullptr)
		, m_Usb(nullptr)
	{
	}

	VirtualInputDevice::~VirtualInputDevice()
	{
	}

	void VirtualInputDevice::update()
	{
		if (m_Joystick == nullptr)
		{
			return;
		}

		InputProcessorBase* processor = m_Joystick->getInputProcessor();
		if (processor == nullptr)
		{
			return;
		}

		XInputState state = { 0 };

		if (!mapToXinput(state, processor->getBindings()) ||
			m_Usb == nullptr)
		{
			return;
		}

		QVector<uint8_t> input(28);

		input[0] = 0x1C;
		input[4] = m_Index;
		input[9] = 0x14;

		for (size_t i = 0; i < (size_t)XInputState::Button::_COUNT; ++i)
		{
			if (state.buttonState[i])
			{
				input[10 + (i / 7)] = (uint8_t)(1 << i);
			}
		}

		input[12] = (uint8_t)(state.axisState[(size_t)XInputState::Axis::LeftTrigger] * 255.0f);
		input[13] = (uint8_t)(state.axisState[(size_t)XInputState::Axis::RightTrigger] * 255.0f);

		QVector<uint8_t> output(8);

		m_Usb->write(0x2A400C, input, output);
	}

	bool VirtualInputDevice::mapToXinput(XInputState& state, const QHash<QString, InputProcessorBase::InputBinding>& bindings)
	{
		state.buttonState[(uint8_t)XInputState::Button::A] = bindings["X"].digitalValue;

		return true;
	}

	void VirtualInputDevice::setJoystick(RawInputJoystick& joystick)
	{
		m_Joystick = &joystick;
	}

	void VirtualInputDevice::setUsbDevice(UsbDevice& usb)
	{
		m_Usb = &usb;
	}

}; // namespace Pitstop