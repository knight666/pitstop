#pragma once

#include "Base/Main.h"
#include "Input/Usb/UsbDevice.h"

namespace Pitstop {

	struct XInputState;

	class UsbController
	{

	public:

		UsbController(QSharedPointer<ConfigurationManager> configuration);
		~UsbController();

		HANDLE getHubHandle() const { return m_HubHandle; }

		UsbDevicePtr createDevice(uint8_t index = (uint8_t)-1);
		UsbDevicePtr createDevice(const QJsonObject& serialized);

		UsbDevicePtr getDeviceByIndex(uint8_t index);

		bool initialize();

	private:

		QSharedPointer<ConfigurationManager> m_Configuration;
		HDEVINFO m_HubInfo;
		QString m_HubPath;
		HANDLE m_HubHandle;
		QVector<UsbDevicePtr> m_Devices;

	}; // class UsbController

}; // namespace Pitstop