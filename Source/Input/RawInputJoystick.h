#pragma once

#include "Base/Main.h"

namespace Pitstop {

	class ProcessorBase;
	class RawInputManager;

	class RawInputJoystick
	{

	public:

		enum class Type
		{
			Raw,
			XInput,
		};

		RawInputJoystick(RawInputManager& manager, HANDLE handle, const RID_DEVICE_INFO& info, HWND window, const QString& name);
		~RawInputJoystick();

		const QString& getDescription() const { return m_Description; }

		const QString& getCategory() const { return m_Category; }

		Type getType() const { return m_Type; }

		HANDLE getHandle() const { return m_Handle; }

		const RAWINPUTDEVICE& getDevice() const { return m_Device; }

		const RID_DEVICE_INFO_HID& getInfo() const { return m_Info.hid; }

		const QString& getDevicePath() const { return m_DevicePath; }

		const GUID& getGuid() const { return m_GUID; }

		bool setup();

		bool process(const RAWINPUT& message);

	private:

		bool retrieveFromRegistry(QString& target, const QString& path, const QString& keyName);

		QString findDevicePath(const GUID& guid);

	private:

		RawInputManager& m_Manager;
		QString m_Description;
		QString m_Category;
		Type m_Type;
		HANDLE m_Handle;
		RAWINPUTDEVICE m_Device;
		RID_DEVICE_INFO m_Info;
		QString m_DevicePath;
		GUID m_GUID;
		ProcessorBase* m_Processor;

	}; // class RawInputJoystick

}; // namespace Pitstop