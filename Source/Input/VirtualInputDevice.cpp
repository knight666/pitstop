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

	HANDLE VirtualInputDevice::getJoystickHandle() const
	{
		return (m_Joystick != nullptr) ? m_Joystick->getHandle() : NULL;
	}

	void VirtualInputDevice::setJoystick(RawInputJoystick& joystick)
	{
		m_Joystick = &joystick;
	}

	void VirtualInputDevice::setUsbDevice(UsbDevice& usb)
	{
		m_Usb = &usb;
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
		input[4] = m_Index + 1;
		input[9] = 0x14;

		for (size_t i = 0; i < (size_t)XInputState::Button::_COUNT; ++i)
		{
			if (state.buttonState[i])
			{
				input[10 + (i / 8)] = (uint8_t)(1 << (i % 8));
			}
		}

		input[12] = (uint8_t)(state.axisState[(size_t)XInputState::Axis::LeftTrigger] * 255.0f);
		input[13] = (uint8_t)(state.axisState[(size_t)XInputState::Axis::RightTrigger] * 255.0f);

		uint16_t left_stick[2] = {
			state.axisState[(size_t)XInputState::Axis::LeftStickHorizontal] * (float)UINT16_MAX,
			state.axisState[(size_t)XInputState::Axis::LeftStickVertical] * (float)UINT16_MAX,
		};

		input[14] = (uint8_t)left_stick[0] & 0xFF;
		input[15] = (uint8_t)(left_stick[0] >> 8) & 0xFF;
		input[16] = (uint8_t)left_stick[1] & 0xFF;
		input[17] = (uint8_t)(left_stick[1] >> 8) & 0xFF;

		QVector<uint8_t> output(8);

		m_Usb->write(0x2A400C, input, output);
	}

	bool VirtualInputDevice::mapToXinput(XInputState& state, const QHash<QString, InputProcessorBase::InputBinding>& bindings)
	{
		state.buttonState[(uint8_t)XInputState::Button::A] = bindings["RightStickDown"].digitalValue;
		state.buttonState[(uint8_t)XInputState::Button::B] = bindings["RightStickRight"].digitalValue;
		state.buttonState[(uint8_t)XInputState::Button::X] = bindings["RightStickLeft"].digitalValue;
		state.buttonState[(uint8_t)XInputState::Button::Y] = bindings["RightStickUp"].digitalValue;
		state.axisState[(uint8_t)XInputState::Axis::LeftStickHorizontal] = bindings["Wheel"].analogValue;

		return true;
	}

}; // namespace Pitstop