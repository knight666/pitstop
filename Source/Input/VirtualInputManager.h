#pragma once

#include "Base/Main.h"

#include "Input/VirtualInputDevice.h"
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

		VirtualInputDevicePtr createDevice();

		VirtualInputDevicePtr getDeviceByIndex(uint8_t index) const;

		virtual bool serialize(QJsonObject& target, size_t version) override;
		virtual bool deserialize(const QJsonObject& source, size_t version) override;

	private:

		RawInputManager& m_RawInput;
		UsbController& m_UsbController;
		QVector<VirtualInputDevicePtr> m_Devices;

	}; // class VirtualInputManager

}; // namespace Pitstop