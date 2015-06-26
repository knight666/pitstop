#pragma once

#include "Base/Main.h"

namespace Pitstop {

	class ProcessorBase;

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

		const QString& getDescription() const { return m_Description; }

		const QString& getCategory() const { return m_Category; }

		Type getType() const { return m_Type; }

		HANDLE getHandle() const { return m_Handle; }

		const RAWINPUTDEVICE& getDevice() const { return m_Device; }

		const RID_DEVICE_INFO_HID& getInfo() const { return m_Info.hid; }

		const QString& getGuid() const { return m_GUID; }

	private:

		void extractStringProperties();

	private:

		QString m_Description;
		QString m_Category;
		Type m_Type;
		HANDLE m_Handle;
		RAWINPUTDEVICE m_Device;
		RID_DEVICE_INFO m_Info;
		QString m_DeviceIdentifier;
		QString m_GUID;
		ProcessorBase* m_Processor;

	}; // class RawInputJoystick

}; // namespace Pitstop