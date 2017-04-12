#include "Input/RawInput/RawInputJoystick.h"

#include <QtCore/QRegularExpression>

#include "Input/Container/ContainerManager.h"
#include "Input/Process/InputProcessorBase.h"
#include "Input/RawInput/RawInputManager.h"

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
		, m_InputProcessor(nullptr)
	{
		memset(&m_Info, 0, sizeof(m_Info));

		memset(&m_Device, 0, sizeof(m_Device));
		m_Device.hwndTarget = window;

		memset(&m_DeviceInfoData, 0, sizeof(SP_DEVINFO_DATA));
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

	const QString& RawInputJoystick::getIdentifier() const
	{
		return m_Container->getIdentifier();
	}

	bool RawInputJoystick::setInputProcessor(InputProcessorBase* processor)
	{
		if (m_InputProcessor == processor)
		{
			return true;
		}

		if (processor != nullptr &&
			!processor->setup(this))
		{
			PS_LOG_ERROR(RawInputJoystick) << "Failed to setup input processor for device. "
				<< "(VID: " << QString::number(m_VendorIdentifier, 16)
				<< " PID: " << QString::number(m_ProductIdentifier, 16)
				<< ")";

			return false;
		}

		m_InputProcessor = processor;

		return true;
	}

	bool RawInputJoystick::setup(ContainerManager& containers, const QString& devicePath)
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

		m_DevicePath = devicePath;
		m_Description.clear();
		m_InstancePath.clear();
		m_Type = Type::RawInput;
		m_XinputIndex = (uint8_t)-1;
		m_VirtualIndex = (uint8_t)-1;

		if (match_path.cap(3).size() > 0 &&
			match_path.cap(4) == "IG_")
		{
			// Extract XInput identifier

			m_Type = RawInputJoystick::Type::XInput;
			m_XinputIndex = (uint8_t)match_path.cap(5).toUInt(nullptr, 16);
		}

		m_VendorIdentifier = (uint16_t)match_path.cap(1).toUInt(nullptr, 16);
		m_ProductIdentifier = (uint16_t)match_path.cap(2).toUInt(nullptr, 16);

		m_Guid = stringToGuid(match_path.cap(7));

		m_InstancePath = devicePath.mid(4);
		m_InstancePath.replace(QRegExp("#\\{[A-Fa-f0-9]{8}-[A-Fa-f0-9]{4}-[A-Fa-f0-9]{4}-[A-Fa-f0-9]{4}-[A-Fa-f0-9]{12}\\}"), "");
		m_InstancePath.replace("#", "\\");

		// Get container

		m_Container = containers.findContainerByGuidAndInstanceIdentifier(m_Guid, m_InstancePath);
		if (m_Container == nullptr)
		{
			PS_LOG_ERROR(RawInputJoystick) << "Failed to get container for device path. (path \"" << devicePath << "\")";

			return false;
		}

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

		for (DWORD i = 0;
			::SetupDiEnumDeviceInfo(
				m_DeviceInfo,
				i,
				&m_DeviceInfoData) == TRUE;
			++i)
		{
			DWORD device_instance_size = 0;

			::SetupDiGetDeviceInstanceIdW(
				m_DeviceInfo,
				&m_DeviceInfoData,
				nullptr,
				0,
				&device_instance_size);

			if (device_instance_size == 0)
			{
				continue;
			}

			QVector<WCHAR> device_instance_data(device_instance_size);

			if (::SetupDiGetDeviceInstanceIdW(
				m_DeviceInfo,
				&m_DeviceInfoData,
				&device_instance_data[0],
				device_instance_data.size(),
				&device_instance_size) == FALSE)
			{
				continue;
			}

			QString device_instance_identifier = QString::fromUtf16(&device_instance_data[0]);
			if (m_InstancePath.compare(device_instance_identifier, Qt::CaseInsensitive) == 0)
			{
				instance_found = true;

				break;
			}
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

		// Check if the device is a virtual controller

		QString container_location = m_Container->getRegistryProperty<QString>(SPDRP_LOCATION_INFORMATION);

		QRegExp match_virtual("SCP Virtual.+\\#([0-9]+)");
		if (match_virtual.indexIn(container_location) >= 0)
		{
			m_Type = Type::Virtual;

			m_VirtualIndex = (uint8_t)match_path.cap(1).toUInt();
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

		auto input_processor = m_Manager.createInputProcessor(*this);
		if (!setInputProcessor(input_processor))
		{
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