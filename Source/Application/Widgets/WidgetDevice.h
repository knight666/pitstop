#pragma once

#include "Base/Main.h"
#include "Input/VirtualInputDevice.h"

#include "ui_WidgetDevice.h"

namespace Pitstop {

	class RawInputManager;
	class UsbController;

	class WidgetDevice
		: public QWidget
	{

		Q_OBJECT

	public:

		WidgetDevice(
			RawInputManager& rawInput,
			UsbController& usbController,
			VirtualInputDevicePtr device,
			QWidget* parent = nullptr,
			Qt::WindowFlags flags = 0);
		~WidgetDevice();

	private slots:

		void slotJoystickCreated(RawInputJoystickPtr joystick);
		void slotJoystickConnected(RawInputJoystickPtr joystick, bool connected);

		void slotJoystickChanged(RawInputJoystickPtr joystick);

		void slotUsbDeviceChanged(UsbDevicePtr usb);
		void slotUsbDeviceConnectionChanged(bool connected);

		void on_cmbJoystick_currentIndexChanged(int index);
		void on_btnConnect_pressed();

	private:

		void setupDevice(VirtualInputDevicePtr device);
		void updateJoysticks();
		void updateThumbnail();
		void updateConnection();

	private:

		RawInputManager& m_RawInput;
		UsbController& m_UsbController;
		VirtualInputDevicePtr m_Device;

		Ui_DeviceForm m_Form;

	}; // class WidgetDevice

	typedef QSharedPointer<WidgetDevice> WidgetDevicePtr;

}; // namespace Pitstop