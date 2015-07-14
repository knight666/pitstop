#include "Input/RawInputJoystick.h"

#include <QtCore/QRegularExpression>

#include "Input/Process/InputProcessorBase.h"
#include "Input/RawInputManager.h"

namespace Pitstop {

	RawInputJoystick::RawInputJoystick(RawInputManager& manager, HWND window)
		: m_Manager(manager)
		, m_Type(Type::Raw)
		, m_VendorIdentifier(0)
		, m_ProductIdentifier(0)
		, m_XinputIndex((uint8_t)-1)
		, m_Connected(false)
		, m_Handle(NULL)
		, m_FileHandle(NULL)
		, m_DeviceInfo(NULL)
		, m_InputProcessor(nullptr)
	{
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

	bool RawInputJoystick::setup(const QString& devicePath)
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
		m_DevicePath = devicePath;
		m_UniquePath = devicePath;
		m_InstancePath.clear();
		m_Type = Type::Raw;
		m_XinputIndex = (uint8_t)-1;

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

		// Category

		retrieveFromRegistry(
			m_Category,
			QString("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\DeviceDisplayObjects\\InterfaceInformation\\") + guidToString(m_Guid),
			"Category");

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

		QString tests[] = {
			getRegistryProperty<QString>(SPDRP_CLASS),
			getRegistryProperty<QString>(SPDRP_CLASSGUID),
			getRegistryProperty<QString>(SPDRP_BASE_CONTAINERID),
			getRegistryProperty<QString>(SPDRP_DEVICEDESC),
			getRegistryProperty<QString>(SPDRP_DRIVER),
		};

		QStringList hardware_ids = getRegistryProperty<QStringList>(SPDRP_HARDWAREID);

		DWORD flags[] = {
			SPDRP_DEVICEDESC, SPDRP_HARDWAREID, SPDRP_COMPATIBLEIDS,
			SPDRP_UNUSED0, SPDRP_SERVICE, SPDRP_UNUSED1,
			SPDRP_UNUSED2, SPDRP_CLASS, SPDRP_CLASSGUID,
			SPDRP_DRIVER, SPDRP_CONFIGFLAGS, SPDRP_MFG,
			SPDRP_FRIENDLYNAME, SPDRP_LOCATION_INFORMATION, SPDRP_PHYSICAL_DEVICE_OBJECT_NAME,
			SPDRP_CAPABILITIES, SPDRP_UI_NUMBER, SPDRP_UPPERFILTERS,
			SPDRP_LOWERFILTERS, SPDRP_BUSTYPEGUID, SPDRP_LEGACYBUSTYPE,
			SPDRP_BUSNUMBER, SPDRP_ENUMERATOR_NAME, SPDRP_SECURITY,
			SPDRP_SECURITY_SDS, SPDRP_DEVTYPE, SPDRP_EXCLUSIVE,
			SPDRP_CHARACTERISTICS, SPDRP_ADDRESS, SPDRP_UI_NUMBER_DESC_FORMAT,
			SPDRP_DEVICE_POWER_DATA, SPDRP_REMOVAL_POLICY, SPDRP_REMOVAL_POLICY_HW_DEFAULT,
			SPDRP_REMOVAL_POLICY_OVERRIDE, SPDRP_INSTALL_STATE, SPDRP_LOCATION_PATHS,
			SPDRP_BASE_CONTAINERID
		};
		QString flag_output[sizeof(flags) / sizeof(DWORD)] = { 0 };

		for (DWORD flag_index = 0; flag_index < sizeof(flags) / sizeof(DWORD); ++flag_index)
		{
			DWORD registry_type = 0;
			DWORD required_size = 0;

			::SetupDiGetDeviceRegistryPropertyW(
				m_DeviceInfo,
				&m_DeviceInfoData,
				flags[flag_index],
				&registry_type,
				NULL,
				0,
				&required_size);
			if (required_size > 0)
			{
				QVector<BYTE> property_data((int)required_size);

				BOOL registry_result = ::SetupDiGetDeviceRegistryPropertyW(
					m_DeviceInfo,
					&m_DeviceInfoData,
					flags[flag_index],
					NULL,
					&property_data[0],
					property_data.size(),
					NULL);
				if (registry_result == TRUE)
				{
					BYTE* property_data_ptr = &property_data[0];

					QString registry_string = QString::fromUtf16((const ushort*)&property_data[0]);

					flag_output[flag_index] = registry_string;

					int i = 0;
				}
			}
		}

		// Match HID ContainerID to USB ContainerID

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
		if (m_Type == Type::Raw)
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

	bool RawInputJoystick::getRegistryProperty(DWORD key, QVector<BYTE>& output, DWORD& keyType)
	{
		DWORD required_size = 0;

		::SetupDiGetDeviceRegistryPropertyW(
			m_DeviceInfo,
			&m_DeviceInfoData,
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
			m_DeviceInfo,
			&m_DeviceInfoData,
			key,
			NULL,
			&output[0],
			output.size(),
			NULL);

		return (registry_result == TRUE);
	}

}; // namespace Pitstop