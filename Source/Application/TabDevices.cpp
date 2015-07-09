#include "Application/TabDevices.h"

#include "Application/Application.h"
#include "Input/Usb/UsbController.h"
#include "Input/RawInputManager.h"
#include "Input/VirtualInputManager.h"

namespace Pitstop {

	TabDevices::TabDevices(QWidget* parent /*= nullptr*/, Qt::WindowFlags flags /*= 0*/)
		: QWidget(parent, flags)
		, m_RawInput(nullptr)
		, m_UsbController(nullptr)
		, m_VirtualInput(nullptr)
	{
		m_Form.setupUi(this);

		m_Form.scrlDevicesContents->setLayout(new QVBoxLayout(m_Form.scrlDevices));
	}

	TabDevices::~TabDevices()
	{
	}

	void TabDevices::setup(
		RawInputManager& rawInput,
		UsbController& usb,
		VirtualInputManager& virtualInput)
	{
		m_UsbController = &usb;
		m_RawInput = &rawInput;
		m_VirtualInput = &virtualInput;
	}

	void TabDevices::on_btnAdd_pressed()
	{
		VirtualInputDevicePtr device = m_VirtualInput->createDevice();

		UsbDevicePtr usb = m_UsbController->createDevice();
		device->setUsbDevice(usb);

		WidgetDevicePtr device_widget(
			new WidgetDevice(
				*m_RawInput,
				device,
				m_Form.scrlDevicesContents));

		QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(m_Form.scrlDevicesContents->layout());
		if (layout != nullptr)
		{
			layout->addWidget(device_widget.data());
		}

		m_Devices.push_back(device_widget);
	}

}; // namespace Pitstop