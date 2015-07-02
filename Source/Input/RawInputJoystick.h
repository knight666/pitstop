#pragma once

#include "Base/Main.h"

namespace Pitstop {

	class InputProcessorBase;
	class RawInputManager;

	class RawInputJoystick
		: public QObject
	{

		Q_OBJECT

	public:

		enum class Type
		{
			Raw,
			XInput,
		};

		RawInputJoystick(RawInputManager& manager, HANDLE handle, const RID_DEVICE_INFO& info, HWND window, const QString& name);
		~RawInputJoystick();

		bool isConnected() { return m_Connected; }
		void setConnected(HANDLE handle, bool value);

		const QString& getDescription() const { return m_Description; }

		const QString& getCategory() const { return m_Category; }

		Type getType() const { return m_Type; }

		uint16_t getVendorIdentifier() const { return m_VendorIdentifier; }

		uint16_t getProductIdentifier() const { return m_ProductIdentifier; }

		HANDLE getHandle() const { return m_Handle; }

		const RAWINPUTDEVICE& getDevice() const { return m_Device; }

		const RID_DEVICE_INFO_HID& getInfo() const { return m_Info.hid; }

		const QString& getDevicePath() const { return m_DevicePath; }

		const GUID& getGuid() const { return m_GUID; }

		InputProcessorBase* getInputProcessor() { return m_InputProcessor; }

		bool setup();

		bool process(const RAWINPUT& message);

	signals:

		void signalJoystickInput(RawInputJoystick* joystick, bool processed);

	private:

		bool retrieveFromRegistry(QString& target, const QString& path, const QString& keyName);

		QString findDevicePath(const GUID& guid);

	private:

		RawInputManager& m_Manager;
		bool m_Connected;
		QString m_Description;
		QString m_Category;
		Type m_Type;
		uint16_t m_VendorIdentifier;
		uint16_t m_ProductIdentifier;
		HANDLE m_Handle;
		RAWINPUTDEVICE m_Device;
		RID_DEVICE_INFO m_Info;
		QString m_DevicePath;
		GUID m_GUID;
		InputProcessorBase* m_InputProcessor;

	}; // class RawInputJoystick

	typedef QSharedPointer<RawInputJoystick> RawInputJoystickPtr;

}; // namespace Pitstop