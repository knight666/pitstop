#pragma once

#include "Base/Main.h"

namespace Pitstop {

	class UsbDevice;

	class UsbController
	{

	public:

		UsbController();
		~UsbController();

		bool initialize();

		UsbDevice* getDeviceByIndex(size_t index);

	private:

		HDEVINFO m_DeviceInfo;
		QString m_DevicePath;
		HANDLE m_DeviceHandle;
		QVector<UsbDevice*> m_Devices;

	}; // class UsbController

}; // namespace Pitstop