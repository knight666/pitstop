#include "Input/RawInputJoystick.h"

#include <QtCore/QRegularExpression>

#include "Input/Container/ContainerDevice.h"
#include "Input/Process/InputProcessorBase.h"
#include "Input/RawInputManager.h"

namespace Pitstop {

	RawInputJoystick::RawInputJoystick(RawInputManager& manager, HWND window)
		: m_Manager(manager)
		, m_Type(Type::RawInput)
		, m_VendorIdentifier(0)
		, m_ProductIdentifier(0)
		, m_XinputIndex((uint8_t)-1)
		, m_Connected(false)
		, m_Handle(NULL)
		, m_FileHandle(NULL)
		, m_DeviceInfo(NULL)
		, m_UsbInfo(NULL)
		, m_InputProcessor(nullptr)
	{
		memset(&m_Info, 0, sizeof(m_Info));

		memset(&m_Device, 0, sizeof(m_Device));
		m_Device.hwndTarget = window;

		memset(&m_DeviceInfoData, 0, sizeof(SP_DEVINFO_DATA));

		memset(&m_UsbInfoData, 0, sizeof(SP_DEVINFO_DATA));
	}

	RawInputJoystick::~RawInputJoystick()
	{
		if (m_FileHandle != NULL)
		{
			::CloseHandle(m_FileHandle);
			m_FileHandle = NULL;
		}

		delete m_InputProcessor;
	}

	void RawInputJoystick::setConnected(HANDLE handle, bool value)
	{
		if (m_Connected == value)
		{
			return;
		}

		if (value)
		{
			PS_LOG_INFO(RawInputJoystick) << "\"" << getDescription() << "\" was connected. (handle: " << m_Handle << " -> " << handle << ")";
		}
		else
		{
			PS_LOG_INFO(RawInputJoystick) << "\"" << getDescription() << "\" was disconnected.";
		}

		m_Handle = handle;
		m_Connected = value;

		emit signalConnected(*this, m_Connected);
	}

	void RawInputJoystick::setDescription(const QString& value)
	{
		m_Description = value;

		emit signalPropertyChanged();
	}

	bool RawInputJoystick::setup(QSharedPointer<ContainerDevice> container, const QString& devicePath)
	{
		// Extract properties from device path

		QRegExp match_path(
			"\\\\\\\\?\\HID#"
			"VID_([0-9A-Fa-f]+)"
			"&PID_([0-9A-Fa-f]+)"
			"(&([A-Za-z]+_?)([0-9A-Fa-f]+))?"
			"(\\#[0-9A-Fa-f\\&]+\\#)"
			"(\\{[A-Fa-f0-9]{8}-[A-Fa-f0-9]{4}-[A-Fa-f0-9]{4}-[A-Fa-f0-9]{4}-[A-Fa-f0-9]{12}\\})");

		if (match_path.indexIn(devicePath) < 0)
		{
			PS_LOG_ERROR(RawInputJoystick) << "Failed to extract properties from device path. (" << devicePath << ")";

			return false;
		}

		m_Description.clear();
		m_Container = container;
		m_DevicePath = devicePath;
		m_UniquePath = devicePath;
		m_InstancePath.clear();
		m_Type = Type::RawInput;
		m_XinputIndex = (uint8_t)-1;
		m_VirtualIndex = (uint8_t)-1;

		if (match_path.cap(3).size() > 0)
		{
			if (match_path.cap(4) == "IG_")
			{
				// Extract XInput identifier

				m_Type = RawInputJoystick::Type::XInput;
				m_XinputIndex = (uint8_t)match_path.cap(5).toUInt(nullptr, 16);
			}

			m_UniquePath.replace(match_path.cap(3), "");
		}

		m_VendorIdentifier = (uint16_t)match_path.cap(1).toUInt(nullptr, 16);
		m_ProductIdentifier = (uint16_t)match_path.cap(2).toUInt(nullptr, 16);

		m_Guid = stringToGuid(match_path.cap(7));

		m_InstancePath = devicePath.mid(4);
		m_InstancePath.replace(QRegExp("#\\{[A-Fa-f0-9]{8}-[A-Fa-f0-9]{4}-[A-Fa-f0-9]{4}-[A-Fa-f0-9]{4}-[A-Fa-f0-9]{12}\\}"), "");
		m_InstancePath.replace("#", "\\");

		// Open file handle

		if (m_FileHandle != NULL)
		{
			::CloseHandle(m_FileHandle);
		}

		m_FileHandle = ::CreateFileW(
			m_DevicePath.utf16(),
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			0,
			NULL);
		if (m_FileHandle == NULL)
		{
			PS_LOG_ERROR(RawInputJoystick) << "Failed to open file handle to device. (\"" << devicePath << "\")";

			return false;
		}

		// Retrieve device information

		m_DeviceInfo = ::SetupDiGetClassDevsW(
			&m_Guid,
			NULL,
			NULL,
			DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
		if (m_DeviceInfo == NULL)
		{
			PS_LOG_ERROR(RawInputJoystick) << "Failed to retrieve device info. (GUID: \"" << guidToString(m_Guid) << "\")";

			return false;
		}

		memset(&m_DeviceInfoData, 0, sizeof(SP_DEVINFO_DATA));
		m_DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

		// Find instance of class

		bool instance_found = false;

		DWORD member_index = 0;

		while (::SetupDiEnumDeviceInfo(
			m_DeviceInfo,
			member_index,
			&m_DeviceInfoData) == TRUE)
		{
			DWORD device_instance_size = 0;

			if (::SetupDiGetDeviceInstanceIdW(
				m_DeviceInfo,
				&m_DeviceInfoData,
				nullptr,
				0,
				&device_instance_size) == FALSE &&
				device_instance_size > 0)
			{
				QVector<WCHAR> device_instance_data(device_instance_size);

				if (::SetupDiGetDeviceInstanceIdW(
					m_DeviceInfo,
					&m_DeviceInfoData,
					&device_instance_data[0],
					device_instance_data.size(),
					&device_instance_size) == TRUE)
				{
					QString device_instance_identifier = QString::fromUtf16(&device_instance_data[0]);
					if (m_InstancePath.compare(device_instance_identifier, Qt::CaseInsensitive) == 0)
					{
						instance_found = true;

						break;
					}
				}
			}

			member_index++;
		}

		if (!instance_found)
		{
			PS_LOG_ERROR(RawInputJoystick) << "Failed to find device instance. (GUID: \"" << guidToString(m_Guid) << "\")";

			return false;
		}

		// Description

		QVector<wchar_t> device_name_data(128);

		bool translated = ::HidD_GetProductString(
			m_FileHandle,
			&device_name_data[0],
			device_name_data.size() * sizeof(wchar_t)) == TRUE;

		if (translated)
		{
			m_Description = QString::fromUtf16(&device_name_data[0]);

			translated = !m_Description.isEmpty();
		}

		if (!translated)
		{
			// Fallback, get device description from driver information

			m_Description = getRegistryProperty<QString>(SPDRP_DEVICEDESC);
		}

		// Find matching USB device

		QString hid_container = getRegistryProperty<QString>(SPDRP_BASE_CONTAINERID, DeviceClass::HID);
		bool container_found = false;

		m_UsbInfo = ::SetupDiGetClassDevsW(
			NULL,
			L"USB",
			NULL,
			DIGCF_ALLCLASSES | DIGCF_PRESENT);

		memset(&m_UsbInfoData, 0, sizeof(SP_DEVINFO_DATA));
		m_UsbInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

		if (m_UsbInfo != INVALID_HANDLE_VALUE)
		{
			for (DWORD i = 0;
				::SetupDiEnumDeviceInfo(
					m_UsbInfo,
					i,
					&m_UsbInfoData) == TRUE;
				++i)
			{
				QString usb_container = getRegistryProperty<QString>(SPDRP_BASE_CONTAINERID, DeviceClass::USB);
				if (usb_container == hid_container)
				{
					container_found = true;

					break;
				}
			}
		}

		if (container_found)
		{
			QString usb_location = getRegistryProperty<QString>(SPDRP_LOCATION_INFORMATION, DeviceClass::USB);

			// Check if the device is a virtual controller

			QRegExp match_virtual("SCP Virtual.+\\#([0-9]+)");
			if (match_virtual.indexIn(usb_location) >= 0)
			{
				m_Type = Type::Virtual;

				m_VirtualIndex = (uint8_t)match_path.cap(1).toUInt();
			}
		}
		else
		{
			m_UsbInfo = NULL;

			memset(&m_UsbInfoData, 0, sizeof(SP_DEVINFO_DATA));
			m_UsbInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
		}

		// Get thumbnail

		m_Thumbnail = m_Manager.getJoystickThumbnail(m_VendorIdentifier, m_ProductIdentifier);

		emit signalPropertyChanged();

		return true;
	}

	bool RawInputJoystick::initialize(HANDLE handle, const RID_DEVICE_INFO& info)
	{
		m_Handle = handle;
		m_Connected = false;

		m_Info = info;

		// Setup device

		m_Device.dwFlags = RIDEV_DEVNOTIFY;
		if (m_Type == Type::RawInput)
		{
			// Ensure input is received even when the window loses focus

			m_Device.dwFlags |= RIDEV_INPUTSINK;
		}

		m_Device.usUsagePage = info.hid.usUsagePage;
		m_Device.usUsage = info.hid.usUsage;

		// Add input processor

		if (m_InputProcessor != nullptr)
		{
			delete m_InputProcessor;
		}

		m_InputProcessor = m_Manager.createInputProcessor(*this);
		if (m_InputProcessor != nullptr &&
			!m_InputProcessor->setup())
		{
			PS_LOG_ERROR(RawInputJoystick) << "Failed to setup input processor for device. "
				<< "(VID: " << QString::number(m_VendorIdentifier, 16)
				<< " PID: " << QString::number(m_ProductIdentifier, 16)
				<< ")";

			return false;
		}

		setConnected(m_Handle, true);

		return true;
	}

	bool RawInputJoystick::process(const RAWINPUT& message)
	{
		bool result = false;

		if (m_Type != Type::XInput &&
			m_InputProcessor != nullptr)
		{
			result = m_InputProcessor->process(message);
		}

		emit signalJoystickInput(this, result);

		return result;
	}

	bool RawInputJoystick::serialize(QJsonObject& target, size_t version)
	{
		target["path"] = m_DevicePath;
		target["description"] = m_Description;

		return true;
	}

	bool RawInputJoystick::getRegistryProperty(DeviceClass deviceClass, DWORD key, QVector<BYTE>& output, DWORD& keyType)
	{
		HDEVINFO info = NULL;
		SP_DEVINFO_DATA* info_data = nullptr;

		switch (deviceClass)
		{

		case DeviceClass::USB:
			info = m_UsbInfo;
			info_data = &m_UsbInfoData;
			break;

		case DeviceClass::HID:
		default:
			info = m_DeviceInfo;
			info_data = &m_DeviceInfoData;
			break;

		}

		if (info == NULL)
		{
			return false;
		}

		DWORD required_size = 0;

		::SetupDiGetDeviceRegistryPropertyW(
			info,
			info_data,
			key,
			&keyType,
			NULL,
			0,
			&required_size);
		if (required_size == 0)
		{
			return false;
		}

		output.resize((int)required_size);

		BOOL registry_result = ::SetupDiGetDeviceRegistryPropertyW(
			info,
			info_data,
			key,
			NULL,
			&output[0],
			output.size(),
			NULL);

		return (registry_result == TRUE);
	}

	bool RawInputJoystick::retrieveFromRegistry(QString& target, const QString& path, const QString& keyName)
	{
		HKEY key = NULL;
		if (::RegOpenKeyExW(
			HKEY_LOCAL_MACHINE,
			(const wchar_t*)path.utf16(),
			0,
			KEY_READ | KEY_WOW64_64KEY,
			&key) != ERROR_SUCCESS)
		{
			return false;
		}

		DWORD length = 0;
		if (::RegQueryValueExW(
			key,
			(const wchar_t*)keyName.utf16(),
			nullptr,
			nullptr,
			nullptr,
			&length) != ERROR_SUCCESS)
		{
			return false;
		}

		QVector<ushort> data((int)length);
		::RegQueryValueExW(
			key,
			(const wchar_t*)keyName.utf16(),
			nullptr,
			nullptr,
			(BYTE*)&data[0],
			&length);

		target = QString::fromUtf16(&data[0]);

		return true;
	}

}; // namespace Pitstop