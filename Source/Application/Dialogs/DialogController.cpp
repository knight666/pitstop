#include "Application/Dialogs/DialogController.h"

#include "Application/Widgets/WidgetSingleAxis.h"
#include "Input/XInput/XInputDevice.h"

namespace Pitstop {

	DialogController::DialogController(QSharedPointer<XInputDevice> device)
		: m_Device(device)
	{
		m_Form.setupUi(this);

		setWindowTitle(QString("Controller %1").arg(device->getUserIndex() + 1));

		m_Form.wdgAxisLeftTrigger->setLimits(0.0f, UINT8_MAX, 32.0f);
		m_Form.wdgAxisLeftTrigger->setTreshold(XINPUT_GAMEPAD_TRIGGER_THRESHOLD);

		m_Form.wdgAxisRightTrigger->setLimits(0.0f, UINT8_MAX, 32.0f);
		m_Form.wdgAxisRightTrigger->setTreshold(XINPUT_GAMEPAD_TRIGGER_THRESHOLD);

		m_Form.wdgLeftStick->setTitle("Left Stick");
		m_Form.wdgLeftStick->setRange(INT16_MAX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);

		m_Form.wdgRightStick->setTitle("Right Stick");
		m_Form.wdgRightStick->setRange(INT16_MAX, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);

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

		m_Form.wdgAxisLeftTrigger->setValue((float)device->getState().bLeftTrigger);

		m_Form.wdgAxisRightTrigger->setValue((float)device->getState().bRightTrigger);

		m_Form.wdgLeftStick->setValue(
			(float)device->getState().sThumbLX,
			(float)device->getState().sThumbLY);

		m_Form.wdgRightStick->setValue(
			(float)device->getState().sThumbRX,
			(float)device->getState().sThumbRY);
	}

}; // namespace Pitstop