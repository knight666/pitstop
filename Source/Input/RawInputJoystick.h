#pragma once

#include "Base/Main.h"

namespace Pitstop {

	class RawInputJoystick
	{

	public:

		RawInputJoystick(HANDLE handle, const RID_DEVICE_INFO& info, HWND window, const QString& name);
		~RawInputJoystick();

		const RAWINPUTDEVICE& GetDevice() const { return m_Device; }

	private:

		QString translateName(const QString& name) const;

	private:

		HANDLE m_Handle;
		RAWINPUTDEVICE m_Device;
		RID_DEVICE_INFO m_Info;
		QString m_DeviceName;
		QString m_TranslatedName;

	}; // class RawInputJoystick

}; // namespace Pitstop