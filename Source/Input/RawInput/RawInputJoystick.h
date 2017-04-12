#pragma once

#include <Core/Main.h>
#include <Core/Serialization/ISerializable.h>

namespace Pitstop {

	class ContainerDevice;
	class ContainerManager;
	class InputProcessorBase;
	class RawInputManager;

	class RawInputJoystick
		: public QObject
		, public ISerializable
	{

		Q_OBJECT
		Q_ENUMS(Type)

	public:

		enum class Type
		{
			RawInput,
			XInput,
			Virtual
		};

		RawInputJoystick(RawInputManager& manager, HWND window);
		~RawInputJoystick();

		bool isConnected() { return m_Connected; }
		void setConnected(HANDLE handle, bool value);

		uint8_t getXinputIndex() const { return m_XinputIndex; }
		void setXinputIndex(uint8_t value) { m_XinputIndex = value; }

		uint8_t getVirtualIndex() const { return m_VirtualIndex; }

		const QString& getDescription() const { return m_Description; }
		void setDescription(const QString& value);

		Type getType() const { return m_Type; }

		uint16_t getVendorIdentifier() const { return m_VendorIdentifier; }

		uint16_t getProductIdentifier() const { return m_ProductIdentifier; }

		HANDLE getHandle() const { return m_Handle; }

		QSharedPointer<ContainerDevice> getContainer() const { return m_Container; }

		const RAWINPUTDEVICE& getDevice() const { return m_Device; }

		const RID_DEVICE_INFO_HID& getInfo() const { return m_Info.hid; }

		const QString& getDevicePath() const { return m_DevicePath; }

		const QString& getIdentifier() const;

		const QString& getInstancePath() const { return m_InstancePath; }

		const GUID& getGuid() const { return m_Guid; }

		InputProcessorBase* getInputProcessor() const { return m_InputProcessor; }
		bool setInputProcessor(InputProcessorBase* processor);

		QSharedPointer<QImage> getThumbnail() const { return m_Thumbnail; }

		template <typename ValueType>
		ValueType getRegistryProperty(DWORD key);

		bool setup(ContainerManager& containers, const QString& devicePath);
		bool initialize(HANDLE handle, const RID_DEVICE_INFO& info);

		bool process(const RAWINPUT& message);

		virtual bool serialize(QJsonObject& target, size_t version) override;

	signals:

		void signalPropertyChanged();
		void signalConnected(RawInputJoystick& joystick, bool connected);
		void signalJoystickInput(RawInputJoystick* joystick, bool processed);

	private:

		bool retrieveFromRegistry(QString& target, const QString& path, const QString& keyName);

	private:

		RawInputManager& m_Manager;
		bool m_Connected;
		uint8_t m_XinputIndex;
		uint8_t m_VirtualIndex;
		QString m_Description;
		Type m_Type;
		uint16_t m_VendorIdentifier;
		uint16_t m_ProductIdentifier;
		HANDLE m_Handle;
		HANDLE m_FileHandle;
		HDEVINFO m_DeviceInfo;
		SP_DEVINFO_DATA m_DeviceInfoData;
		QSharedPointer<ContainerDevice> m_Container;
		RAWINPUTDEVICE m_Device;
		RID_DEVICE_INFO m_Info;
		QString m_DevicePath;
		QString m_InstancePath;
		GUID m_Guid;
		InputProcessorBase* m_InputProcessor;
		QSharedPointer<QImage> m_Thumbnail;

	}; // class RawInputJoystick

	template <typename ValueType>
	inline ValueType RawInputJoystick::getRegistryProperty(DWORD key)
	{
		return deviceGetRegistryProperty<ValueType>(m_DeviceInfo, m_DeviceInfoData, key);
	}

	inline QTextStream& operator << (QTextStream& stream, const RawInputJoystick::Type& type)
	{
		int type_index = RawInputJoystick::staticMetaObject.indexOfEnumerator("Type");
		QMetaEnum type_enum = RawInputJoystick::staticMetaObject.enumerator(type_index);

		stream << type_enum.valueToKey((int)type);

		return stream;
	}

}; // namespace Pitstop