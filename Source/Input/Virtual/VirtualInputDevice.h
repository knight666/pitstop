#pragma once

#include <Core/Main.h>
#include "Input/Process/InputProcessorBase.h"
#include "Serialization/ConfigurationEventDispatcher.h"
#include "Serialization/ISerializable.h"

namespace Pitstop {

	class ConfigurationManager;
	class RawInputJoystick;
	class UsbController;
	class UsbDevice;
	class VirtualInputManager;
	struct XInputState;

	class VirtualInputDevice
		: public ConfigurationEventDispatcher
		, public ISerializable
	{

		Q_OBJECT

		friend class VirtualInputManager;

	public:

		VirtualInputDevice(
			QSharedPointer<ConfigurationManager>& configuration,
			VirtualInputManager& virtualInput,
			uint8_t index);
		~VirtualInputDevice();

		HANDLE getJoystickHandle() const;

		const QSharedPointer<RawInputJoystick>& getJoystick() const { return m_Joystick; }
		void setJoystick(QSharedPointer<RawInputJoystick> joystick);

		const QSharedPointer<UsbDevice>& getUsbDevice() const { return m_Usb; }
		void setUsbDevice(QSharedPointer<UsbDevice> usb);

		virtual bool serialize(QJsonObject& target, size_t version) override;
		bool deserialize(RawInputManager& rawInput, UsbController& usbController, const QJsonObject& source, size_t version);

	signals:

		void signalJoystickChanged(QSharedPointer<RawInputJoystick> joystick);
		void signalUsbDeviceChanged(QSharedPointer<UsbDevice> usb);

	public slots:

		void slotJoystickInput(RawInputJoystick* joystick, bool processed);

	private:

		bool mapToXinput(XInputState& state, const QHash<QString, InputProcessorBase::InputBinding>& bindings);

	private:

		VirtualInputManager& m_VirtualInput;
		uint8_t m_Index;
		QSharedPointer<RawInputJoystick> m_Joystick;
		QSharedPointer<UsbDevice> m_Usb;

	}; // class VirtualInputDevice

}; // namespace Pitstop