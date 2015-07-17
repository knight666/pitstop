#include "Application/TabDevices.h"

#include "Application/Application.h"
#include "Input/RawInput/RawInputManager.h"
#include "Input/Usb/UsbController.h"
#include "Input/Virtual/VirtualInputManager.h"

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
		disconnect(
			this, SLOT(slotVirtualDeviceCreated(QSharedPointer<VirtualInputDevice>)));
	}

	void TabDevices::setup(
		RawInputManager& rawInput,
		UsbController& usb,
		VirtualInputManager& virtualInput)
	{
		m_RawInput = &rawInput;

		m_UsbController = &usb;

		m_VirtualInput = &virtualInput;

		connect(
			m_VirtualInput, SIGNAL(signalVirtualDeviceCreated(QSharedPointer<VirtualInputDevice>)),
			this, SLOT(slotVirtualDeviceCreated(QSharedPointer<VirtualInputDevice>)));
	}

	void TabDevices::on_btnAdd_pressed()
	{
		QSharedPointer<VirtualInputDevice> device = m_VirtualInput->createDevice();
		device->setUsbDevice(m_UsbController->createDevice());
	}

	void TabDevices::slotVirtualDeviceCreated(QSharedPointer<VirtualInputDevice> device)
	{
		WidgetDevicePtr device_widget(
			new WidgetDevice(
				*m_RawInput,
				*m_UsbController,
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