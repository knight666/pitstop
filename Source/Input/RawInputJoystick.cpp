#include "Input/RawInputJoystick.h"

#include <SetupAPI.h>
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

		// Extract properties

		m_DevicePath = devicePath;
		m_UniquePath = devicePath;
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

		// Get category

		retrieveFromRegistry(
			m_Category,
			QString("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\DeviceDisplayObjects\\InterfaceInformation\\") + guidToString(m_Guid),
			"Category");

		// Get description

		m_Description = devicePath;

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

}; // namespace Pitstop