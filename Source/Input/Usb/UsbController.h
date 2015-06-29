#pragma once

#include "Base/Main.h"

namespace Pitstop {

	class UsbDevice;
	struct XInputState;

	class UsbController
	{

	public:

		UsbController();
		~UsbController();

		HANDLE getHubHandle() const { return m_HubHandle; }

		UsbDevice* getDeviceByIndex(uint8_t index);

		bool initialize();

	private:

		HDEVINFO m_HubInfo;
		QString m_HubPath;
		HANDLE m_HubHandle;
		QVector<UsbDevice*> m_Devices;

	}; // class UsbController

}; // namespace Pitstop