#include "Application/Dialogs/DialogController.h"

#include "Application/Widgets/WidgetNeedleIndicator.h"
#include "Input/XInput/XInputDevice.h"

namespace Pitstop {

	DialogController::DialogController(QSharedPointer<XInputDevice> device)
		: m_Device(device)
	{
		m_Form.setupUi(this);

		setWindowTitle(QString("Controller %1").arg(device->getUserIndex() + 1));

		m_Form.wdgAxisLeftTrigger->setProperties(0.0f, 255.0f, 32.0f);
		m_Form.wdgAxisRightTrigger->setProperties(0.0f, 255.0f, 32.0f);

		connect(
			device.data(), SIGNAL(signalChanged()),
			this, SLOT(slotDeviceChanged()));
	}

	DialogController::~DialogController()
	{
		disconnect(
			this, SLOT(slotDeviceChanged()));
	}

	void DialogController::slotDeviceChanged()
	{
		XInputDevice* device = qobject_cast<XInputDevice*>(sender());
		if (device == nullptr)
		{
			return;
		}

		m_Form.lblAxisLeftTrigger->setText(QString::number(device->getState().bLeftTrigger));
		m_Form.wdgAxisLeftTrigger->setValue((float)device->getState().bLeftTrigger);

		m_Form.lblAxisRightTrigger->setText(QString::number(device->getState().bRightTrigger));
		m_Form.wdgAxisRightTrigger->setValue((float)device->getState().bRightTrigger);
	}

}; // namespace Pitstop