#pragma once

#include "Base/Main.h"
#include "Application/Widgets/WidgetDevice.h"

#include "ui_TabDevices.h"

namespace Pitstop {

	class RawInputManager;
	class UsbController;
	class VirtualInputManager;

	class TabDevices
		: public QWidget
	{

		Q_OBJECT

	public:

		TabDevices(QWidget* parent = nullptr, Qt::WindowFlags flags = 0);
		~TabDevices();

		void setup(
			RawInputManager& rawInput,
			UsbController& usb,
			VirtualInputManager& virtualInput);

	public slots:

		void on_btnAdd_pressed();

	private:

		Ui_TabDevices m_Form;

		RawInputManager* m_RawInput;
		UsbController* m_UsbController;
		VirtualInputManager* m_VirtualInput;
		QVector<WidgetDevicePtr> m_Devices;

	}; // class TabDevices

	typedef QSharedPointer<TabDevices> TabDevicesPtr;

}; // namespace Pitstop