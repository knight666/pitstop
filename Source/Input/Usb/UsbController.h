#pragma once

#include "Base/Main.h"
#include "Input/Usb/UsbDevice.h"

namespace Pitstop {

	struct XInputState;

	class UsbController
	{

	public:

		UsbController();
		~UsbController();

		HANDLE getHubHandle() const { return m_HubHandle; }

		UsbDevicePtr createDevice();

		UsbDevicePtr getDeviceByIndex(uint8_t index);

		bool initialize();

	private:

		HDEVINFO m_HubInfo;
		QString m_HubPath;
		HANDLE m_HubHandle;
		QVector<UsbDevicePtr> m_Devices;

	}; // class UsbController

}; // namespace Pitstop