#include "Input/Usb/UsbDevice.h"

#include "Input/Process/InputProcessorBase.h"
#include "Input/Usb/UsbController.h"

namespace Pitstop {

	UsbDevice::UsbDevice(UsbController& controller, uint8_t identifier)
		: m_Controller(controller)
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

		write(value ? 0x002A4000 : 0x002A4004, input, output);

		m_Connected = value;
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
			DWORD last_error = GetLastError();
			PS_LOG_ERROR(UsbDevice) << "Failed to execute command " << command << " on device " << m_Identifier << ". (error: \"" << windowsErrorToString(last_error) << "\" code: " << last_error << ")";

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