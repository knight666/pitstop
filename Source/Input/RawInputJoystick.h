#pragma once

#include "Base/Main.h"
#include "Serialization/ISerializable.h"

namespace Pitstop {

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
		void setDescription(const QString& value);

		const QString& getCategory() const { return m_Category; }

		Type getType() const { return m_Type; }

		uint16_t getVendorIdentifier() const { return m_VendorIdentifier; }

		uint16_t getProductIdentifier() const { return m_ProductIdentifier; }

		HANDLE getHandle() const { return m_Handle; }

		const RAWINPUTDEVICE& getDevice() const { return m_Device; }

		const RID_DEVICE_INFO_HID& getInfo() const { return m_Info.hid; }

		const QString& getDevicePath() const { return m_DevicePath; }

		const QString& getUniquePath() const { return m_UniquePath; }

		const QString& getInstancePath() const { return m_InstancePath; }

		const GUID& getGuid() const { return m_Guid; }

		InputProcessorBase* getInputProcessor() const { return m_InputProcessor; }

		QSharedPointer<QImage> getThumbnail() const { return m_Thumbnail; }

		bool getRegistryProperty(DWORD key, QVector<BYTE>& output, DWORD& keyType);

		template <typename ValueType>
		ValueType getRegistryProperty(DWORD key);

		bool setup(const QString& devicePath);
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
		QString m_Description;
		QString m_Category;
		Type m_Type;
		uint16_t m_VendorIdentifier;
		uint16_t m_ProductIdentifier;
		HANDLE m_Handle;
		HANDLE m_FileHandle;
		HDEVINFO m_DeviceInfo;
		SP_DEVINFO_DATA m_DeviceInfoData;
		RAWINPUTDEVICE m_Device;
		RID_DEVICE_INFO m_Info;
		QString m_DevicePath;
		QString m_UniquePath;
		QString m_InstancePath;
		GUID m_Guid;
		InputProcessorBase* m_InputProcessor;
		QSharedPointer<QImage> m_Thumbnail;

	}; // class RawInputJoystick

	template <>
	inline DWORD RawInputJoystick::getRegistryProperty(DWORD key)
	{
		QVector<BYTE> output;
		DWORD key_type = 0;

		if (!getRegistryProperty(key, output, key_type) ||
			key_type != REG_DWORD)
		{
			return 0;
		}

		return *(DWORD*)&output[0];
	}

	template <>
	inline QString RawInputJoystick::getRegistryProperty(DWORD key)
	{
		QVector<BYTE> output;
		DWORD key_type = 0;
		
		if (!getRegistryProperty(key, output, key_type) ||
			key_type != REG_SZ)
		{
			return QString();
		}

		return QString::fromUtf16((const ushort*)&output[0]);
	}

	template <>
	inline QStringList RawInputJoystick::getRegistryProperty(DWORD key)
	{
		QVector<BYTE> output;
		DWORD key_type = 0;

		if (!getRegistryProperty(key, output, key_type) ||
			key_type != REG_MULTI_SZ)
		{
			return QStringList();
		}

		QStringList result;

		const wchar_t* input = (const wchar_t*)&output[0];
		size_t input_length = wcslen(input);

		while (input_length > 0)
		{
			QString entry = QString::fromUtf16(input, input_length * sizeof(wchar_t));
			result.push_back(entry);

			input += input_length + 1;
			input_length = wcslen(input);
		}

		return result;
	}

	typedef QSharedPointer<RawInputJoystick> RawInputJoystickPtr;

	inline QTextStream& operator << (QTextStream& stream, const RawInputJoystick::Type& type)
	{
		int type_index = RawInputJoystick::staticMetaObject.indexOfEnumerator("Type");
		QMetaEnum type_enum = RawInputJoystick::staticMetaObject.enumerator(type_index);

		stream << type_enum.valueToKey((int)type);

		return stream;
	}

}; // namespace Pitstop