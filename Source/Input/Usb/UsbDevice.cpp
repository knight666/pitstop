#include "Input/Usb/UsbDevice.h"

#include "Input/Process/InputProcessorBase.h"
#include "Input/Usb/UsbController.h"

namespace Pitstop {

	UsbDevice::UsbDevice(UsbController& controller, uint8_t identifier)
		: m_Controller(controller)
		, m_Identifier(identifier)
		, m_PluggedIn(false)
	{
	}

	UsbDevice::~UsbDevice()
	{
	}

	void UsbDevice::setPluggedIn(bool value)
	{
		if (value == m_PluggedIn)
		{
			return;
		}

		QVector<uint8_t> input(16);
		input[0] = 0x10;
		input[4] = m_Identifier;

		QVector<uint8_t> output(8);

		write(value ? 0x002A4000 : 0x002A4004, input, output);

		m_PluggedIn = value;
	}

	bool UsbDevice::write(DWORD command, QVector<uint8_t>& input, QVector<uint8_t>& output)
	{
		/*0x2A400C

		input[0] = 0x1C;
		input[4] = m_Identifier;
		input[9] = 0x14;

		for (size_t i = 0; i < (size_t)XInputState::Button::_COUNT; ++i)
		{
			if ((state.buttonState[i] & ProcessorBase::InputState_Down) != 0)
			{
				input[10 + (i / 7)] = (uint8_t)(1 << i);
			}
		}

		input[12] = (uint8_t)(state.axisState[(size_t)XInputState::Axis::LeftTrigger] * 255.0f);
		input[13] = (uint8_t)(state.axisState[(size_t)XInputState::Axis::RightTrigger] * 255.0f);

		// TODO: Sticks*/

		if (input.size() == 0 ||
			output.size() == 0)
		{
			return false;
		}

		DWORD written = 0;

		if (::DeviceIoControl(
			m_Controller.getHubHandle(),
			command,
			&input[0], (DWORD)input.size(),
			&output[0], (DWORD)output.size(),
			&written,
			nullptr) == FALSE)
		{
			DWORD last_error = ::GetLastError();

			return false;
		}

		return true;
	}

}; // namespace Pitstop