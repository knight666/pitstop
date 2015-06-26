#include "Input/RawInputJoystick.h"

#include <QtCore/QRegularExpression>

namespace Pitstop {

	RawInputJoystick::RawInputJoystick(HANDLE handle, const RID_DEVICE_INFO& info, HWND window, const QString& name)
		: m_Handle(handle)
		, m_Info(info)
		, m_DeviceIdentifier(name)
		, m_Type(Type::Raw)
	{
		memset(&m_Device, 0, sizeof(m_Device));
		m_Device.hwndTarget = window;

		extractStringProperties();
	}

	RawInputJoystick::~RawInputJoystick()
	{
	}

	void RawInputJoystick::extractStringProperties()
	{
		m_GUID.clear();
		m_Category.clear();
		m_Description = m_DeviceIdentifier;

		// Extract GUID

		QRegExp extract_guid("(\\{.+\\})");
		if (extract_guid.indexIn(m_DeviceIdentifier) < 0)
		{
			return;
		}

		m_GUID = extract_guid.cap(1);

		// Extract VID and PID

		QRegExp extract_info("VID_([A-Fa-f0-9]+)&PID_([A-Fa-f0-9]+)");
		if (extract_info.indexIn(m_DeviceIdentifier) < 0)
		{
			return;
		}

		QString vid = extract_info.cap(1);
		QString pid = extract_info.cap(2);

		// Check if managed by XInput

		if (m_DeviceIdentifier.indexOf("IG_") >= 0)
		{
			m_Type = Type::XInput;
		}

		// Get category

		QString category_key_path = QString("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\DeviceDisplayObjects\\InterfaceInformation\\") + m_GUID;

		HKEY category_key = NULL;
		if (::RegOpenKeyExW(HKEY_LOCAL_MACHINE, (const wchar_t*)category_key_path.utf16(), 0, KEY_READ | KEY_WOW64_64KEY, &category_key) == ERROR_SUCCESS)
		{
			DWORD length = 0;
			if (::RegQueryValueExW(category_key, L"Category", nullptr, nullptr, nullptr, &length) == ERROR_SUCCESS)
			{
				QVector<ushort> category_name_data;
				category_name_data.resize((int)length);
				::RegQueryValueExW(category_key, L"Category", nullptr, nullptr, (BYTE*)&category_name_data[0], &length);

				m_Category = QString::fromUtf16(&category_name_data[0], (int)length);
			}
		}

		// Get translated name

		QString description_key_path = QString("SYSTEM\\CurrentControlSet\\Control\\MediaProperties\\PrivateProperties\\Joystick\\OEM\\VID_%1&PID_%2").arg(vid).arg(pid);

		HKEY description_key = NULL;
		if (::RegOpenKeyExW(HKEY_LOCAL_MACHINE, (const wchar_t*)description_key_path.utf16(), 0, KEY_READ | KEY_WOW64_64KEY, &description_key) == ERROR_SUCCESS)
		{
			DWORD length = 0;
			if (::RegQueryValueExW(description_key, L"OEMName", nullptr, nullptr, nullptr, &length) == ERROR_SUCCESS)
			{
				QVector<ushort> description_data;
				description_data.resize((int)length);
				::RegQueryValueExW(description_key, L"OEMName", nullptr, nullptr, (BYTE*)&description_data[0], &length);

				m_Description = QString::fromUtf16(&description_data[0], (int)length);
			}
		}
	}

}; // namespace Pitstop