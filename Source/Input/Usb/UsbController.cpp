#include "Input/Usb/UsbController.h"

namespace Pitstop {

	UsbController::UsbController()
		: m_HubInfo(NULL)
	{
	}

	UsbController::~UsbController()
	{
		PS_LOG_INFO(UsbController) << "Destroying USB controller.";

		qDeleteAll(m_Devices);

		if (m_HubInfo != NULL)
		{
			::SetupDiDestroyDeviceInfoList(m_HubInfo);
			m_HubInfo = NULL;
		}
	}

	UsbDevice* UsbController::getDeviceByIndex(uint8_t index)
	{
		return (index < m_Devices.size()) ? m_Devices[index] : nullptr;
	}

	bool UsbController::initialize()
	{
		PS_LOG_INFO(UsbController) << "Initializing USB controller.";

		GUID guid;
		::CLSIDFromString(L"{F679F562-3164-42CE-A4DB-E7DDBE723909}", &guid);

		m_HubInfo = ::SetupDiGetClassDevsW(
			&guid,
			nullptr,
			nullptr,
			DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
		if (m_HubInfo == NULL)
		{
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
			return false;
		}

		SP_DEVINFO_DATA device_detail_data = { 0 };
		device_detail_data.cbSize = sizeof(SP_DEVINFO_DATA);

		DWORD buffer_size = 0;

		if (::SetupDiGetDeviceInterfaceDetailW(
			m_HubInfo,
			&device_interface_data,
			nullptr,
			0,
			&buffer_size,
			&device_detail_data) == TRUE)
		{
			return false;
		}

		QVector<uint8_t> detail_data;
		detail_data.resize(buffer_size + sizeof(DWORD));

		SP_DEVICE_INTERFACE_DETAIL_DATA_W* detail_data_ptr = (SP_DEVICE_INTERFACE_DETAIL_DATA_W*)&detail_data[0];
		detail_data_ptr->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);

		if (::SetupDiGetDeviceInterfaceDetailW(
			m_HubInfo,
			&device_interface_data,
			(SP_DEVICE_INTERFACE_DETAIL_DATA_W*)&detail_data[0],
			buffer_size,
			&buffer_size,
			&device_detail_data) == FALSE)
		{
			return false;
		}

		m_HubPath = QString::fromUtf16((const ushort*)&detail_data_ptr->DevicePath[0]);

		m_HubHandle = ::CreateFileW(
			m_HubPath.utf16(),
			GENERIC_WRITE | GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			nullptr,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
			0);
		if (m_HubHandle == NULL)
		{
			return false;
		}

		for (size_t i = 0; i < 4; ++i)
		{
			UsbDevice* device = new UsbDevice(*this, (uint8_t)i + 1);

			m_Devices.push_back(device);
		}

		return true;
	}

}; // namespace Pitstop