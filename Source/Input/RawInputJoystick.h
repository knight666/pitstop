#pragma once

#include "Base/Main.h"

namespace Pitstop {

	class RawInputJoystick
	{

	public:

		enum class Type
		{
			Raw,
			XInput,
		};

		RawInputJoystick(HANDLE handle, const RID_DEVICE_INFO& info, HWND window, const QString& name);
		~RawInputJoystick();

		const RAWINPUTDEVICE& getDevice() const { return m_Device; }

		Type getType() const { return m_Type; }

		const QString& getDescription() const { return m_Description; }

		const QString& getCategory() const { return m_Category; }

	private:

		void extractStringProperties();

	private:

		Type m_Type;
		HANDLE m_Handle;
		RAWINPUTDEVICE m_Device;
		RID_DEVICE_INFO m_Info;
		QString m_DeviceIdentifier;
		QString m_GUID;
		QString m_Category;
		QString m_Description;

	}; // class RawInputJoystick

}; // namespace Pitstop