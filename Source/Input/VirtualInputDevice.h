#pragma once

#include "Base/Main.h"
#include "Input/Process/InputProcessorBase.h"
#include "Input/Usb/UsbDevice.h"
#include "Input/RawInputJoystick.h"
#include "Serialization/ISerializable.h"

namespace Pitstop {

	class VirtualInputManager;
	struct XInputState;

	class VirtualInputDevice
		: public QObject
		, public ISerializable
	{

		Q_OBJECT

		friend class VirtualInputManager;

	public:

		VirtualInputDevice(VirtualInputManager& manager, uint8_t index);
		~VirtualInputDevice();

		HANDLE getJoystickHandle() const;

		const RawInputJoystickPtr& getJoystick() const { return m_Joystick; }
		void setJoystick(RawInputJoystickPtr joystick);

		const UsbDevicePtr& getUsbDevice() const { return m_Usb; }
		void setUsbDevice(UsbDevicePtr usb);

		virtual bool serialize(QJsonObject& target, size_t version = SERIALIZATION_VERSION) override;
		virtual bool deserialize(const QJsonObject& source, size_t version = SERIALIZATION_VERSION) override;

	public slots:

		void slotJoystickInput(RawInputJoystick* joystick, bool processed);

	private:

		bool mapToXinput(XInputState& state, const QHash<QString, InputProcessorBase::InputBinding>& bindings);

	private:

		VirtualInputManager& m_Manager;
		uint8_t m_Index;
		RawInputJoystickPtr m_Joystick;
		UsbDevicePtr m_Usb;

	}; // class VirtualInputDevice

	typedef QSharedPointer<VirtualInputDevice> VirtualInputDevicePtr;

}; // namespace Pitstop