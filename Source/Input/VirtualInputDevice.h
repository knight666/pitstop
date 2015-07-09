#pragma once

#include "Base/Main.h"
#include "Input/Process/InputProcessorBase.h"
#include "Input/Usb/UsbDevice.h"
#include "Input/RawInputJoystick.h"
#include "Serialization/ConfigurationEventDispatcher.h"
#include "Serialization/ISerializable.h"

namespace Pitstop {

	class ConfigurationManager;
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

		const RawInputJoystickPtr& getJoystick() const { return m_Joystick; }
		void setJoystick(RawInputJoystickPtr joystick);

		const UsbDevicePtr& getUsbDevice() const { return m_Usb; }
		void setUsbDevice(UsbDevicePtr usb);

		virtual bool serialize(QJsonObject& target, size_t version) override;
		bool deserialize(RawInputManager& rawInput, UsbController& usbController, const QJsonObject& source, size_t version);

	public slots:

		void slotJoystickInput(RawInputJoystick* joystick, bool processed);

	private:

		bool mapToXinput(XInputState& state, const QHash<QString, InputProcessorBase::InputBinding>& bindings);

	private:

		VirtualInputManager& m_VirtualInput;
		uint8_t m_Index;
		RawInputJoystickPtr m_Joystick;
		UsbDevicePtr m_Usb;

	}; // class VirtualInputDevice

	typedef QSharedPointer<VirtualInputDevice> VirtualInputDevicePtr;

}; // namespace Pitstop