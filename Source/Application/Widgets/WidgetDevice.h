#pragma once

#include "Base/Main.h"
#include "Input/VirtualInputDevice.h"

#include "ui_WidgetDevice.h"

namespace Pitstop {

	class WidgetDevice
		: public QWidget
	{

		Q_OBJECT

	public:

		WidgetDevice(VirtualInputDevicePtr device, QWidget* parent = nullptr, Qt::WindowFlags flags = 0);
		~WidgetDevice();

	private:

		void setupDevice(VirtualInputDevicePtr device);

	private:

		Ui_DeviceForm m_Form;
		VirtualInputDevicePtr m_Device;

	}; // class WidgetDevice

	typedef QSharedPointer<WidgetDevice> WidgetDevicePtr;

}; // namespace Pitstop