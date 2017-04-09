#pragma once

#include <Core/Main.h>

#include "ui_WidgetDevice.h"

namespace Pitstop {

	class RawInputJoystick;
	class RawInputManager;
	class UsbController;
	class UsbDevice;
	class VirtualInputDevice;

	class WidgetDevice
		: public QWidget
	{

		Q_OBJECT

	public:

		WidgetDevice(
			RawInputManager& rawInput,
			UsbController& usbController,
			QSharedPointer<VirtualInputDevice> device,
			QWidget* parent = nullptr,
			Qt::WindowFlags flags = 0);
		~WidgetDevice();

	private slots:

		void slotJoystickCreated(QSharedPointer<RawInputJoystick> joystick);
		void slotJoystickConnected(QSharedPointer<RawInputJoystick> joystick, bool connected);

		void slotJoystickChanged(QSharedPointer<RawInputJoystick> joystick);

		void slotUsbDeviceChanged(QSharedPointer<UsbDevice> usb);
		void slotUsbDeviceConnectionChanged(bool connected);

		void on_cmbJoystick_currentIndexChanged(int index);
		void on_btnConnect_pressed();

	private:

		void setupDevice(QSharedPointer<VirtualInputDevice> device);
		void updateJoysticks();
		void updateThumbnail();
		void updateConnection();

	private:

		RawInputManager& m_RawInput;
		UsbController& m_UsbController;
		QSharedPointer<VirtualInputDevice> m_Device;

		Ui_DeviceForm m_Form;

	}; // class WidgetDevice

	typedef QSharedPointer<WidgetDevice> WidgetDevicePtr;

}; // namespace Pitstop