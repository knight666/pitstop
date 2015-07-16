#include "Input/Usb/UsbDevice.h"

#include "Input/Process/InputProcessorBase.h"
#include "Input/Usb/UsbController.h"

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

	bool UsbDevice::serialize(QJsonObject& target, size_t version)
	{
		target["identifier"] = m_Identifier;
		target["connected"] = m_Connected;

		return true;
	}

}; // namespace Pitstop