#include "Input/RawInputJoystick.h"

#include <QtCore/QRegularExpression>

#include "Input/Process/ProcessorBase.h"

namespace Pitstop {

	RawInputJoystick::RawInputJoystick(HANDLE handle, const RID_DEVICE_INFO& info, HWND window, const QString& name)
		: m_Handle(handle)
		, m_Info(info)
		, m_DeviceIdentifier(name)
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
		if (extract_guid.indexIn(m_DeviceIdentifier) < 0)
		{
			return false;
		}

		m_GUID = extract_guid.cap(1);

		// Extract VID and PID

		QRegExp extract_info("VID_([A-Fa-f0-9]+)&PID_([A-Fa-f0-9]+)");
		if (extract_info.indexIn(m_DeviceIdentifier) < 0)
		{
			return false;
		}

		QString vid = extract_info.cap(1);
		QString pid = extract_info.cap(2);

		// Check if managed by XInput

		if (m_DeviceIdentifier.indexOf("IG_") >= 0)
		{
			m_Type = Type::XInput;
		}

		// Get category

		retrieveFromRegistry(
			m_Category,
			QString("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\DeviceDisplayObjects\\InterfaceInformation\\") + m_GUID,
			"Category");

		// Get translated name

		if (!retrieveFromRegistry(
			m_Description,
			QString("SYSTEM\\CurrentControlSet\\Control\\MediaProperties\\PrivateProperties\\Joystick\\OEM\\VID_%1&PID_%2").arg(vid).arg(pid),
			"OEMName"))
		{
			QString path = m_DeviceIdentifier;
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

}; // namespace Pitstop