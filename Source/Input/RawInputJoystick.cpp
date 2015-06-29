#include "Input/RawInputJoystick.h"

#include <SetupAPI.h>
#include <QtCore/QRegularExpression>

#include "Input/Process/ProcessorBase.h"

namespace Pitstop {

	RawInputJoystick::RawInputJoystick(RawInputManager& manager, HANDLE handle, const RID_DEVICE_INFO& info, HWND window, const QString& name)
		: m_Manager(manager)
		, m_Handle(handle)
		, m_Info(info)
		, m_DevicePath(name)
		, m_Description(name)
		, m_Type(Type::Raw)
		, m_Processor(nullptr)
	{
		memset(&m_Device, 0, sizeof(m_Device));
		m_Device.usUsagePage = info.hid.usUsagePage;
		m_Device.usUsage = info.hid.usUsage;
		m_Device.dwFlags = 0;
		m_Device.hwndTarget = window;
	}

	RawInputJoystick::~RawInputJoystick()
	{
		delete m_Processor;
	}

	bool RawInputJoystick::setup()
	{
		// Extract GUID

		QRegExp extract_guid("(\\{.+\\})");
		if (extract_guid.indexIn(m_DevicePath) < 0)
		{
			return false;
		}

		QString guid_string = extract_guid.cap(1);

		// Extract VID and PID

		QRegExp extract_info("VID_([A-Fa-f0-9]+)&PID_([A-Fa-f0-9]+)");
		if (extract_info.indexIn(m_DevicePath) < 0)
		{
			return false;
		}

		QString vid = extract_info.cap(1);
		QString pid = extract_info.cap(2);

		// Check if managed by XInput

		if (m_DevicePath.indexOf("IG_") >= 0)
		{
			m_Type = Type::XInput;
		}

		// Get category

		retrieveFromRegistry(
			m_Category,
			QString("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\DeviceDisplayObjects\\InterfaceInformation\\") + guid_string,
			"Category");

		// Get translated name

		if (!retrieveFromRegistry(
			m_Description,
			QString("SYSTEM\\CurrentControlSet\\Control\\MediaProperties\\PrivateProperties\\Joystick\\OEM\\VID_%1&PID_%2").arg(vid).arg(pid),
			"OEMName"))
		{
			QString path = m_DevicePath;
			path.replace(0, 4, "SYSTEM\\CurrentControlSet\\Enum\\");
			path.replace('#', '\\');
			path.replace(QRegExp("\\{.+\\}"), "");

			if (retrieveFromRegistry(m_Description, path, "DeviceDesc"))
			{
				// Save only the name from the device description

				int last_semicolon = m_Description.lastIndexOf(';');
				if (last_semicolon >= 0)
				{
					m_Description = m_Description.mid(
						last_semicolon + 1,
						m_Description.length() - last_semicolon);
				}
			}
		}

		::CLSIDFromString(guid_string.utf16(), &m_GUID);
		QString path = findDevicePath(m_GUID);

		// Add input processor

		m_Processor = new ProcessorBase(*this);
		return m_Processor->setup();
	}

	bool RawInputJoystick::process(const RAWINPUT& message)
	{
		return (m_Processor != nullptr) ? m_Processor->process(message) : false;
	}

	bool RawInputJoystick::retrieveFromRegistry(QString& target, const QString& path, const QString& keyName)
	{
		HKEY key = NULL;
		if (::RegOpenKeyExW(HKEY_LOCAL_MACHINE, (const wchar_t*)path.utf16(), 0, KEY_READ | KEY_WOW64_64KEY, &key) != ERROR_SUCCESS)
		{
			return false;
		}

		DWORD length = 0;
		if (::RegQueryValueExW(key, (const wchar_t*)keyName.utf16(), nullptr, nullptr, nullptr, &length) != ERROR_SUCCESS)
		{
			return false;
		}

		QVector<ushort> data;
		data.resize((int)length);
		::RegQueryValueExW(key, (const wchar_t*)keyName.utf16(), nullptr, nullptr, (BYTE*)&data[0], &length);

		target = QString::fromUtf16(&data[0], (int)length);

		return true;
	}

	QString RawInputJoystick::findDevicePath(const GUID& guid)
	{
		QString device_path;

		SP_DEVICE_INTERFACE_DATA device_interface_data = { 0 };
		device_interface_data.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

		HDEVINFO device_info = ::SetupDiGetClassDevsW(
			&guid,
			nullptr,
			nullptr,
			DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

		DWORD member_index = 0;
		while (::SetupDiEnumDeviceInterfaces(
			device_info,
			nullptr,
			&guid,
			member_index,
			&device_interface_data) == TRUE)
		{
			SP_DEVINFO_DATA device_detail_data = { 0 };
			device_detail_data.cbSize = sizeof(SP_DEVINFO_DATA);

			DWORD buffer_size = 0;

			if (::SetupDiGetDeviceInterfaceDetailW(
				device_info,
				&device_interface_data,
				nullptr,
				0,
				&buffer_size,
				&device_detail_data) == FALSE)
			{
				QVector<uint8_t> detail_data;
				detail_data.resize(buffer_size + sizeof(DWORD));

				SP_DEVICE_INTERFACE_DETAIL_DATA_W* detail_data_ptr = (SP_DEVICE_INTERFACE_DETAIL_DATA_W*)&detail_data[0];
				detail_data_ptr->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);

				if (::SetupDiGetDeviceInterfaceDetailW(
					device_info,
					&device_interface_data,
					(SP_DEVICE_INTERFACE_DETAIL_DATA_W*)&detail_data[0],
					buffer_size,
					&buffer_size,
					&device_detail_data) == TRUE)
				{
					device_path = QString::fromUtf16((const ushort*)&detail_data_ptr->DevicePath[0]);

					break;
				}
			}

			DWORD last_error = ::GetLastError();

			member_index++;
		}

		return device_path;
	}

}; // namespace Pitstop