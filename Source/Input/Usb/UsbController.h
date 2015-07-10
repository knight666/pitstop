#pragma once

#include "Base/Main.h"
#include "Input/Usb/UsbDevice.h"

#define USB_COMMAND_CONNECT            CTL_CODE(FILE_DEVICE_BUS_EXTENDER, 0x0000, METHOD_BUFFERED, FILE_READ_ACCESS)
#define USB_COMMAND_DISCONNECT         CTL_CODE(FILE_DEVICE_BUS_EXTENDER, 0x0001, METHOD_BUFFERED, FILE_READ_ACCESS)
#define USB_COMMAND_WRITE              CTL_CODE(FILE_DEVICE_BUS_EXTENDER, 0x0003, METHOD_BUFFERED, FILE_READ_ACCESS)

namespace Pitstop {

	class RawInputManager;
	struct XInputState;

	class UsbController
		: public QObject
	{

		Q_OBJECT

	public:

		UsbController(
			QSharedPointer<ConfigurationManager> configuration,
			RawInputManager& rawInput);
		~UsbController();

		HANDLE getHubHandle() const { return m_HubHandle; }

		UsbDevicePtr createDevice(uint8_t index = (uint8_t)-1);
		UsbDevicePtr createDevice(const QJsonObject& serialized);

		UsbDevicePtr getDeviceByIndex(uint8_t index);

		bool initialize();

	public slots:

		void slotUsbDeviceConnectionChanged(bool connected);

	private:

		QSharedPointer<ConfigurationManager> m_Configuration;
		RawInputManager& m_RawInput;
		HDEVINFO m_HubInfo;
		QString m_HubPath;
		GUID m_HubGuid;
		HANDLE m_HubHandle;
		QVector<UsbDevicePtr> m_Devices;

	}; // class UsbController

}; // namespace Pitstop