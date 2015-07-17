#pragma once

#include "Base/Main.h"

#include "Input/RawInput/RawInputManager.h"
#include "Input/Usb/UsbController.h"
#include "Input/Virtual/VirtualInputDevice.h"
#include "Serialization/ConfigurationBase.h"

namespace Pitstop {

	class RawInputManager;

	class VirtualInputManager
		: public ConfigurationBase
	{

		Q_OBJECT

	public:

		VirtualInputManager(
			QSharedPointer<ConfigurationManager> configuration,
			RawInputManager& rawInput,
			UsbController& usbController);
		~VirtualInputManager();

		QSharedPointer<VirtualInputDevice> createDevice();

		QSharedPointer<VirtualInputDevice> getDeviceByIndex(uint8_t index) const;

		virtual bool serialize(QJsonObject& target, size_t version) override;
		virtual bool deserialize(const QJsonObject& source, size_t version) override;

	signals:

		void signalVirtualDeviceCreated(QSharedPointer<VirtualInputDevice> device);

	private:

		RawInputManager& m_RawInput;
		UsbController& m_UsbController;
		QVector<QSharedPointer<VirtualInputDevice>> m_Devices;

	}; // class VirtualInputManager

}; // namespace Pitstop