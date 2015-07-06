#pragma once

#include "Base/Main.h"

namespace Pitstop {

	class InputProcessorBase;
	class RawInputManager;

	class RawInputJoystick
		: public QObject
	{

		Q_OBJECT
		Q_ENUMS(Type)

	public:

		enum class Type
		{
			Raw,
			XInput,
		};

		RawInputJoystick(RawInputManager& manager, HWND window);
		~RawInputJoystick();

		bool isConnected() { return m_Connected; }
		void setConnected(HANDLE handle, bool value);

		uint8_t getXinputIndex() const { return m_XinputIndex; }
		void setXinputIndex(uint8_t value) { m_XinputIndex = value; }

		const QString& getDescription() const { return m_Description; }

		const QString& getCategory() const { return m_Category; }

		Type getType() const { return m_Type; }

		uint16_t getVendorIdentifier() const { return m_VendorIdentifier; }

		uint16_t getProductIdentifier() const { return m_ProductIdentifier; }

		HANDLE getHandle() const { return m_Handle; }

		const RAWINPUTDEVICE& getDevice() const { return m_Device; }

		const RID_DEVICE_INFO_HID& getInfo() const { return m_Info.hid; }

		const QString& getDevicePath() const { return m_DevicePath; }

		const GUID& getGuid() const { return m_Guid; }
		const QString& getGuidString() const { return m_GuidString; }

		InputProcessorBase* getInputProcessor() const { return m_InputProcessor; }

		QSharedPointer<QImage> getThumbnail() const { return m_Thumbnail; }

		bool setup(HANDLE handle, const RID_DEVICE_INFO& info, const QString& path);

		bool process(const RAWINPUT& message);

	signals:

		void signalConnected(RawInputJoystick& joystick, bool connected);
		void signalJoystickInput(RawInputJoystick* joystick, bool processed);

	private:

		bool retrieveFromRegistry(QString& target, const QString& path, const QString& keyName);

		QString findDevicePath(const GUID& guid);

	private:

		RawInputManager& m_Manager;
		bool m_Connected;
		uint8_t m_XinputIndex;
		QString m_Description;
		QString m_Category;
		Type m_Type;
		uint16_t m_VendorIdentifier;
		uint16_t m_ProductIdentifier;
		HANDLE m_Handle;
		RAWINPUTDEVICE m_Device;
		RID_DEVICE_INFO m_Info;
		QString m_DevicePath;
		GUID m_Guid;
		QString m_GuidString;
		InputProcessorBase* m_InputProcessor;
		QSharedPointer<QImage> m_Thumbnail;

	}; // class RawInputJoystick

	typedef QSharedPointer<RawInputJoystick> RawInputJoystickPtr;

	inline QTextStream& operator << (QTextStream& stream, const RawInputJoystick::Type& type)
	{
		int type_index = RawInputJoystick::staticMetaObject.indexOfEnumerator("Type");
		QMetaEnum type_enum = RawInputJoystick::staticMetaObject.enumerator(type_index);

		stream << type_enum.valueToKey((int)type);

		return stream;
	}

}; // namespace Pitstop