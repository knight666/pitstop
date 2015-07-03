#include "Input/RawInputJoystick.h"

#include <SetupAPI.h>
#include <QtCore/QRegularExpression>

#include "Input/Process/InputProcessorBase.h"
#include "Input/RawInputManager.h"

namespace Pitstop {

	RawInputJoystick::RawInputJoystick(RawInputManager& manager, HWND window)
		: m_Manager(manager)
		, m_XinputIndex((uint8_t)-1)
		, m_Connected(false)
		, m_VendorIdentifier(0)
		, m_ProductIdentifier(0)
		, m_Handle(NULL)
		, m_Type(Type::Raw)
		, m_InputProcessor(nullptr)
	{
		memset(&m_Device, 0, sizeof(m_Device));
		m_Device.hwndTarget = window;
	}

	RawInputJoystick::~RawInputJoystick()
	{
		delete m_InputProcessor;
	}

	void RawInputJoystick::setConnected(HANDLE handle, bool value)
	{
		if (m_Connected == value)
		{
			return;
		}

		m_Handle = handle;
		m_Connected = value;

		emit signalConnected(*this, m_Connected);
	}

	bool RawInputJoystick::setup(HANDLE handle, const RID_DEVICE_INFO& info, const QString& path)
	{
		m_Connected = false;
		m_Handle = handle;

		m_Info = info;
		m_Device.usUsagePage = info.hid.usUsagePage;
		m_Device.usUsage = info.hid.usUsage;

		m_DevicePath = path;
		m_Description = path;

		// Extract GUID

		QRegExp extract_guid("(\\{.+\\})");
		if (extract_guid.indexIn(m_DevicePath) < 0)
		{
			return false;
		}

		m_GuidString = extract_guid.cap(1);
		::CLSIDFromString(m_GuidString.utf16(), &m_Guid);

		// Extract VID and PID

		QRegExp extract_info("VID_([A-Fa-f0-9]+)&PID_([A-Fa-f0-9]+)");
		if (extract_info.indexIn(m_DevicePath) < 0)
		{
			return false;
		}

		QString vid = extract_info.cap(1);
		m_VendorIdentifier = vid.toInt(nullptr, 16);

		QString pid = extract_info.cap(2);
		m_ProductIdentifier = pid.toInt(nullptr, 16);

		// Check if managed by XInput

		m_Device.dwFlags = RIDEV_DEVNOTIFY;

		if (m_DevicePath.indexOf("IG_") >= 0)
		{
			m_Type = Type::XInput;
		}
		else
		{
			m_Type = Type::Raw;

			// Ensure input is received even when the window loses focus

			m_Device.dwFlags |= RIDEV_INPUTSINK;
		}

		// Get category

		retrieveFromRegistry(
			m_Category,
			QString("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\DeviceDisplayObjects\\InterfaceInformation\\") + m_GuidString,
			"Category");

		// Get translated name

		bool translated = false;

		HANDLE hid_handle = ::CreateFileW(
			path.utf16(),
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			0,
			NULL);

		if (hid_handle != NULL)
		{
			QVector<ushort> device_name_data(128);

			translated = ::HidD_GetProductString(
				hid_handle,
				&device_name_data[0],
				device_name_data.size() * sizeof(ushort)) == TRUE;

			if (translated)
			{
				m_Description = QString::fromUtf16(
					&device_name_data[0],
					(int)wcslen((const wchar_t*)&device_name_data[0]));

				translated = !m_Description.isEmpty();
			}

			::CloseHandle(hid_handle);
		}

		if (!translated)
		{
			// Fallback, get device description from driver information

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

		// Get thumbnail

		m_Thumbnail = m_Manager.getJoystickThumbnail(m_VendorIdentifier, m_ProductIdentifier);

		// Add input processor

		bool result = true;

		if (m_InputProcessor != nullptr)
		{
			delete m_InputProcessor;
		}

		m_InputProcessor = m_Manager.createInputProcessor(*this);
		if (m_InputProcessor != nullptr)
		{
			result = m_InputProcessor->setup();
		}

		setConnected(m_Handle, true);

		return result;
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