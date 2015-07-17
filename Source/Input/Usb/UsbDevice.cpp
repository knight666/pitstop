#include "Input/Usb/UsbDevice.h"

#include "Input/Process/InputProcessorBase.h"
#include "Input/Usb/UsbController.h"
#include "Input/XInput/XInputState.h"

namespace Pitstop {

	UsbDevice::UsbDevice(
			QSharedPointer<ConfigurationManager> configuration,
			UsbController& controller,
			uint8_t identifier)
		: ConfigurationEventDispatcher(configuration)
		, m_Controller(controller)
		, m_Identifier(identifier)
		, m_Connected(false)
	{
	}

	UsbDevice::~UsbDevice()
	{
		setConnected(false);
	}

	void UsbDevice::setConnected(bool value)
	{
		if (value == m_Connected)
		{
			return;
		}

		PS_LOG_TRACE(UsbDevice) << "setPluggedIn identifier " << m_Identifier << " connected " << value;

		QVector<uint8_t> input(16);
		input[0] = 0x10;
		input[4] = m_Identifier;

		QVector<uint8_t> output;

		if (write(value ? USB_COMMAND_CONNECT : USB_COMMAND_DISCONNECT, input, output))
		{
			m_Connected = value;

			emit signalConnectionChanged(value);
			emit signalSaveConfiguration();
		}
	}

	bool UsbDevice::write(DWORD command, QVector<uint8_t>& input, QVector<uint8_t>& output)
	{
		if (input.size() == 0)
		{
			return false;
		}

		LPVOID input_ptr = &input[0];
		DWORD input_size = input.size();

		LPVOID output_ptr = (output.size() > 0) ? &output[0] : nullptr;
		DWORD output_size = output.size();

		DWORD written = 0;

		if (::DeviceIoControl(
			m_Controller.getHubHandle(),
			command,
			input_ptr, input_size,
			output_ptr, output_size,
			&written,
			nullptr) == FALSE)
		{
			PS_LOG_ERROR(UsbDevice) << "Failed to execute command " << command << " on device " << m_Identifier << "." << PS_LOG_WINDOWS_ERROR;

			return false;
		}

		return true;
	}

	bool UsbDevice::writeXinputState(const XInputState& state)
	{
		if (!isConnected())
		{
			return false;
		}

		QVector<uint8_t> input(28);

		input[0] = 0x1C;
		input[4] = m_Identifier;
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

		return write(USB_COMMAND_WRITE, input, output);
	}

	bool UsbDevice::serialize(QJsonObject& target, size_t version)
	{
		target["identifier"] = m_Identifier;
		target["connected"] = m_Connected;

		return true;
	}

}; // namespace Pitstop