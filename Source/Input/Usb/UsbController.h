#pragma once

#include "Base/Main.h"
#include "Input/Usb/UsbDevice.h"

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
		HANDLE m_HubHandle;
		QVector<UsbDevicePtr> m_Devices;

	}; // class UsbController

}; // namespace Pitstop