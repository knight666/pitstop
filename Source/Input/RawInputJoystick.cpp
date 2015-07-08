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

	RawInputJoystick::RawInputJoystick(RawInputManager& manager, HWND window, const QString& devicePath, const QString& uniquePath, Type type, uint16_t vendor, uint16_t product, const GUID& guid)
		: m_Manager(manager)
		, m_DevicePath(devicePath)
		, m_UniquePath(uniquePath)
		, m_Description(devicePath)
		, m_Type(type)
		, m_VendorIdentifier(vendor)
		, m_ProductIdentifier(product)
		, m_Guid(guid)
		, m_XinputIndex((uint8_t)-1)
		, m_Connected(false)
		, m_Handle(NULL)
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

	QString RawInputJoystick::getGuidString() const
	{
		OLECHAR* guid_string_data;
		::StringFromCLSID(m_Guid, &guid_string_data);
		QString result = QString::fromUtf16(guid_string_data);
		::CoTaskMemFree(guid_string_data);

		return result;
	}

	bool RawInputJoystick::setup(HANDLE handle, const RID_DEVICE_INFO& info)
	{
		m_Handle = handle;
		m_Connected = false;

		m_Info = info;
		m_Device.usUsagePage = info.hid.usUsagePage;
		m_Device.usUsage = info.hid.usUsage;

		m_Description = m_DevicePath;

		// Check if managed by XInput

		m_Device.dwFlags = RIDEV_DEVNOTIFY;
		if (m_Type == Type::Raw)
		{
			// Ensure input is received even when the window loses focus

			m_Device.dwFlags |= RIDEV_INPUTSINK;
		}

		// Get category

		retrieveFromRegistry(
			m_Category,
			QString("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\DeviceDisplayObjects\\InterfaceInformation\\") + getGuidString(),
			"Category");

		// Get translated name

		bool translated = false;

		HANDLE hid_handle = ::CreateFileW(
			m_DevicePath.utf16(),
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
				m_Description = QString::fromUtf16(&device_name_data[0]);

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