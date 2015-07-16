#include "Input/Usb/UsbController.h"

#include "Input/RawInput/RawInputManager.h"

#define MAX_USB_DEVICES 4

namespace Pitstop {

	UsbController::UsbController(
			QSharedPointer<ConfigurationManager> configuration,
			RawInputManager& rawInput)
		: m_Configuration(configuration)
		, m_RawInput(rawInput)
		, m_HubInfo(NULL)
		, m_HubGuid(GUID_NULL)
		, m_HubHandle(NULL)
	{
	}

	UsbController::~UsbController()
	{
		PS_LOG_INFO(UsbController) << "Destroying USB controller.";

		disconnect(
			this, SLOT(slotUsbDeviceConnectionChanged(bool)));

		m_Devices.clear();

		if (m_HubInfo != NULL)
		{
			::SetupDiDestroyDeviceInfoList(m_HubInfo);
			m_HubInfo = NULL;
		}
	}

	UsbDevicePtr UsbController::createDevice(uint8_t index /*= (uint8_t)-1*/)
	{
		UsbDevicePtr device;

		uint8_t identifier = (index >= MAX_USB_DEVICES)
			? m_Devices.size()
			: index;

		if (identifier <= MAX_USB_DEVICES)
		{
			identifier++;

			PS_LOG_INFO(UsbController) << "Creating device " << identifier << ".";

			device = UsbDevicePtr(
				new UsbDevice(
					m_Configuration,
					*this,
					identifier));

			connect(
				device.data(), SIGNAL(signalConnectionChanged(bool)),
				this, SLOT(slotUsbDeviceConnectionChanged(bool)));

			m_Devices.push_back(device);
		}

		return device;
	}

	UsbDevicePtr UsbController::createDevice(const QJsonObject& serialized)
	{
		// Device

		if (!serialized.contains("identifier") ||
			!serialized["identifier"].isDouble())
		{
			PS_LOG_ERROR(UsbController) << "Missing required field \"identifier\".";

			return UsbDevicePtr();
		}

		uint8_t identifier = (uint8_t)serialized["identifier"].toDouble();
		if (identifier < 1 ||
			identifier > MAX_USB_DEVICES)
		{
			PS_LOG_ERROR(UsbController) << "Identifier " << identifier << " is out of range.";

			return UsbDevicePtr();
		}

		UsbDevicePtr device = createDevice(identifier - 1);
		if (device == nullptr)
		{
			PS_LOG_ERROR(UsbController) << "Failed to create device. (identifier " << identifier << ")";

			return device;
		}

		// Connected

		if (serialized.contains("connected") &&
			serialized["connected"].isBool())
		{
			device->setConnected(serialized["connected"].toBool());
		}

		return device;
	}

	UsbDevicePtr UsbController::getDeviceByIndex(uint8_t index)
	{
		return
			(index < m_Devices.size())
				? m_Devices[index]
				: UsbDevicePtr();
	}

	bool UsbController::initialize()
	{
		PS_LOG_INFO(UsbController) << "Initializing USB controller.";

		GUID guid;
		::CLSIDFromString(L"{F679F562-3164-42CE-A4DB-E7DDBE723909}", &guid);

		m_HubInfo = ::SetupDiGetClassDevsW(
			&guid,
			NULL,
			NULL,
			DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
		if (m_HubInfo == NULL)
		{
			PS_LOG_ERROR(UsbController) << "Failed to retrieve handle to virtual USB hub." << PS_LOG_WINDOWS_ERROR;

			return false;
		}

		SP_DEVICE_INTERFACE_DATA device_interface_data = { 0 };
		device_interface_data.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

		if (::SetupDiEnumDeviceInterfaces(
			m_HubInfo,
			nullptr,
			&guid,
			0,
			&device_interface_data) == FALSE)
		{
			PS_LOG_ERROR(UsbController) << "Failed to initialize device interface." << PS_LOG_WINDOWS_ERROR;

			return false;
		}

		m_HubGuid = device_interface_data.InterfaceClassGuid;

		SP_DEVINFO_DATA device_detail_data = { 0 };
		device_detail_data.cbSize = sizeof(SP_DEVINFO_DATA);

		DWORD buffer_size = 0;

		if (::SetupDiGetDeviceInterfaceDetailW(
			m_HubInfo,
			&device_interface_data,
			NULL,
			0,
			&buffer_size,
			&device_detail_data) == TRUE)
		{
			PS_LOG_ERROR(UsbController) << "Failed to retrieve size of device interface." << PS_LOG_WINDOWS_ERROR;

			return false;
		}

		QVector<uint8_t> detail_data;
		detail_data.resize(buffer_size + sizeof(DWORD));

		SP_DEVICE_INTERFACE_DETAIL_DATA_W* detail_data_ptr = (SP_DEVICE_INTERFACE_DETAIL_DATA_W*)&detail_data[0];
		detail_data_ptr->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);

		if (::SetupDiGetDeviceInterfaceDetailW(
			m_HubInfo,
			&device_interface_data,
			detail_data_ptr,
			buffer_size,
			&buffer_size,
			&device_detail_data) == FALSE)
		{
			PS_LOG_ERROR(UsbController) << "Failed to retrieve device interface data." << PS_LOG_WINDOWS_ERROR;

			return false;
		}

		m_HubPath = QString::fromUtf16(
			(const ushort*)&detail_data_ptr->DevicePath[0]);

		m_HubHandle = ::CreateFileW(
			m_HubPath.utf16(),
			GENERIC_WRITE | GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
			NULL);
		if (m_HubHandle == NULL)
		{
			PS_LOG_ERROR(UsbController) << "Failed to open handle to USB hub." << PS_LOG_WINDOWS_ERROR;

			return false;
		}

		// Disconnect devices previously left dangling

		uint8_t input[16] = { 0 };
		input[0] = 0x10;

		for (uint8_t i = 0; i < MAX_USB_DEVICES; ++i)
		{
			input[4] = i;

			DWORD written = 0;

			if (::DeviceIoControl(
				m_HubHandle,
				USB_COMMAND_DISCONNECT,
				input, 16,
				nullptr, 0,
				&written,
				nullptr) != FALSE)
			{
				PS_LOG_INFO(UsbController) << "Disconnecting dangling USB device " << i << ".";
			}
		}

		PS_LOG_INFO(UsbController) << "Initialized.";

		return true;
	}

	void UsbController::slotUsbDeviceConnectionChanged(bool connected)
	{
		if (connected)
		{
			m_RawInput.updateRegisteredDevices();
		}
	}

}; // namespace Pitstop