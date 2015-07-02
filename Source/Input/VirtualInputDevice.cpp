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
		disconnect(
			this, SLOT(slotJoystickInput(RawInputJoystick*, bool)));
	}

	HANDLE VirtualInputDevice::getJoystickHandle() const
	{
		return (m_Joystick != nullptr) ? m_Joystick->getHandle() : NULL;
	}

	void VirtualInputDevice::setJoystick(RawInputJoystick& joystick)
	{
		connect(
			&joystick, SIGNAL(signalJoystickInput(RawInputJoystick*, bool)),
			this, SLOT(slotJoystickInput(RawInputJoystick*, bool)));

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

		int16_t* input_stick = (int16_t*)&input[14];
		input_stick[0] = state.axisState[(size_t)XInputState::Axis::LeftStickHorizontal] * (float)INT16_MAX;
		input_stick[1] = state.axisState[(size_t)XInputState::Axis::LeftStickVertical] * (float)INT16_MAX;
		input_stick[2] = state.axisState[(size_t)XInputState::Axis::RightStickHorizontal] * (float)INT16_MAX;
		input_stick[3] = state.axisState[(size_t)XInputState::Axis::RightStickVertical] * (float)INT16_MAX;

		QVector<uint8_t> output(8);

		m_Usb->write(0x2A400C, input, output);
	}

	void VirtualInputDevice::slotJoystickInput(RawInputJoystick* joystick, bool processed)
	{
		if (joystick != m_Joystick ||
			joystick == nullptr)
		{
			return;
		}

		// Map input to XInput

		XInputState state = { 0 };

		InputProcessorBase* processor = m_Joystick->getInputProcessor();
		if (processor == nullptr)
		{
			return;
		}

		if (!mapToXinput(state, processor->getBindings()))
		{
			return;
		}

		// Send to USB device

		if (m_Usb == nullptr)
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

		int16_t* input_stick = (int16_t*)&input[14];
		input_stick[0] = state.axisState[(size_t)XInputState::Axis::LeftStickHorizontal] * (float)INT16_MAX;
		input_stick[1] = state.axisState[(size_t)XInputState::Axis::LeftStickVertical] * (float)INT16_MAX;
		input_stick[2] = state.axisState[(size_t)XInputState::Axis::RightStickHorizontal] * (float)INT16_MAX;
		input_stick[3] = state.axisState[(size_t)XInputState::Axis::RightStickVertical] * (float)INT16_MAX;

		QVector<uint8_t> output(8);

		m_Usb->write(0x2A400C, input, output);
	}

	bool VirtualInputDevice::mapToXinput(XInputState& state, const QHash<QString, InputProcessorBase::InputBinding>& bindings)
	{
		// Look behind
		state.buttonState[(uint8_t)XInputState::Button::A] = bindings["RightThumbButton"].digitalValue;
		state.buttonState[(uint8_t)XInputState::Button::B] = bindings["RightStickRight"].digitalValue;
		state.buttonState[(uint8_t)XInputState::Button::X] = bindings["RightStickLeft"].digitalValue;
		state.buttonState[(uint8_t)XInputState::Button::Y] = bindings["RightStickUp"].digitalValue;

		// Horn
		state.buttonState[(uint8_t)XInputState::Button::LeftThumb] = bindings["LeftThumbButton"].digitalValue;

		// Change camera
		state.buttonState[(uint8_t)XInputState::Button::Left] = bindings["RightStickDown"].digitalValue;

		float wheel = std::min(std::max(-1.0f, bindings["Wheel"].analogValue * 5.0f), 1.0f);
		state.axisState[(uint8_t)XInputState::Axis::LeftStickHorizontal] = wheel;

		float look_around =
			bindings["RightGearShiftUp"].digitalValue ? 1.0f :
			bindings["LeftGearShiftDown"].digitalValue ? -1.0f :
			0.0f;

		state.axisState[(uint8_t)XInputState::Axis::RightStickHorizontal] = look_around;

		state.axisState[(uint8_t)XInputState::Axis::LeftTrigger] = bindings["LeftFootPaddle"].analogValue;
		state.axisState[(uint8_t)XInputState::Axis::RightTrigger] = bindings["RightFootPaddle"].analogValue;

		return true;
	}

}; // namespace Pitstop