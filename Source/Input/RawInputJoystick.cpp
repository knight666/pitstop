#include "Input/RawInputJoystick.h"

namespace Pitstop {

	RawInputJoystick::RawInputJoystick(HANDLE handle, const RID_DEVICE_INFO& info, HWND window, const QString& name)
		: m_Handle(handle)
		, m_Info(info)
		, m_DeviceName(name)
	{
		memset(&m_Device, 0, sizeof(m_Device));
		m_Device.hwndTarget = window;

		m_TranslatedName = translateName(m_DeviceName);
	}

	RawInputJoystick::~RawInputJoystick()
	{
	}

	QString RawInputJoystick::translateName(const QString& name) const
	{
		return name;
	}

}; // namespace Pitstop