#include "Application/Dialogs/DialogController.h"

#include "Input/XInput/XInputDevice.h"

namespace Pitstop {

	DialogController::DialogController(QSharedPointer<XInputDevice> device)
		: m_Device(device)
	{
		m_Form.setupUi(this);

		setWindowTitle(QString("Controller %1").arg(device->getUserIndex() + 1));

		m_Form.wdgButtonA->setTitle("A");
		m_Form.wdgButtonA->setImage(QPixmap(":/Icons/Resources/Controller-50.png"));

		m_Form.wdgButtonB->setTitle("B");
		m_Form.wdgButtonB->setImage(QPixmap(":/Icons/Resources/Controller-50.png"));

		m_Form.wdgButtonX->setTitle("X");
		m_Form.wdgButtonX->setImage(QPixmap(":/Icons/Resources/Controller-50.png"));

		m_Form.wdgButtonY->setTitle("Y");
		m_Form.wdgButtonY->setImage(QPixmap(":/Icons/Resources/Controller-50.png"));

		m_Form.wdgButtonDpadLeft->setTitle("Left");
		m_Form.wdgButtonDpadLeft->setImage(QPixmap(":/Icons/Resources/Controller-50.png"));

		m_Form.wdgButtonDpadRight->setTitle("Right");
		m_Form.wdgButtonDpadRight->setImage(QPixmap(":/Icons/Resources/Controller-50.png"));

		m_Form.wdgButtonDpadUp->setTitle("Up");
		m_Form.wdgButtonDpadUp->setImage(QPixmap(":/Icons/Resources/Controller-50.png"));

		m_Form.wdgButtonDpadDown->setTitle("Down");
		m_Form.wdgButtonDpadDown->setImage(QPixmap(":/Icons/Resources/Controller-50.png"));

		m_Form.wdgButtonBack->setTitle("Back");
		m_Form.wdgButtonBack->setImage(QPixmap(":/Icons/Resources/Controller-50.png"));

		m_Form.wdgButtonStart->setTitle("Start");
		m_Form.wdgButtonStart->setImage(QPixmap(":/Icons/Resources/Controller-50.png"));

		m_Form.wdgButtonBumperLeft->setTitle("Left Bumper");
		m_Form.wdgButtonBumperLeft->setImage(QPixmap(":/Icons/Resources/Controller-50.png"));

		m_Form.wdgButtonBumperRight->setTitle("Right Bumper");
		m_Form.wdgButtonBumperRight->setImage(QPixmap(":/Icons/Resources/Controller-50.png"));

		m_Form.wdgButtonThumbLeft->setTitle("Left Thumb");
		m_Form.wdgButtonThumbLeft->setImage(QPixmap(":/Icons/Resources/Controller-50.png"));

		m_Form.wdgButtonThumbRight->setTitle("Right Thumb");
		m_Form.wdgButtonThumbRight->setImage(QPixmap(":/Icons/Resources/Controller-50.png"));

		m_Form.wdgTriggerLeft->setTitle("Left Trigger");
		m_Form.wdgTriggerLeft->setRange(
			0.0f,
			UINT8_MAX,
			32.0f,
			XINPUT_GAMEPAD_TRIGGER_THRESHOLD);

		m_Form.wdgTriggerRight->setTitle("Right Trigger");
		m_Form.wdgTriggerRight->setRange(
			0.0f,
			UINT8_MAX,
			32.0f,
			XINPUT_GAMEPAD_TRIGGER_THRESHOLD);

		m_Form.wdgStickLeft->setTitle("Left Stick");
		m_Form.wdgStickLeft->setRange(INT16_MAX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);

		m_Form.wdgStickRight->setTitle("Right Stick");
		m_Form.wdgStickRight->setRange(INT16_MAX, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);

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

		m_Form.wdgButtonA->setValue((device->getState().wButtons & XINPUT_GAMEPAD_A) != 0);
		m_Form.wdgButtonB->setValue((device->getState().wButtons & XINPUT_GAMEPAD_B) != 0);
		m_Form.wdgButtonX->setValue((device->getState().wButtons & XINPUT_GAMEPAD_X) != 0);
		m_Form.wdgButtonY->setValue((device->getState().wButtons & XINPUT_GAMEPAD_Y) != 0);

		m_Form.wdgButtonDpadLeft->setValue((device->getState().wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != 0);
		m_Form.wdgButtonDpadRight->setValue((device->getState().wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0);
		m_Form.wdgButtonDpadUp->setValue((device->getState().wButtons & XINPUT_GAMEPAD_DPAD_UP) != 0);
		m_Form.wdgButtonDpadDown->setValue((device->getState().wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0);

		m_Form.wdgButtonBack->setValue((device->getState().wButtons & XINPUT_GAMEPAD_BACK) != 0);
		m_Form.wdgButtonStart->setValue((device->getState().wButtons & XINPUT_GAMEPAD_START) != 0);

		m_Form.wdgButtonBumperLeft->setValue((device->getState().wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0);
		m_Form.wdgButtonBumperRight->setValue((device->getState().wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0);

		m_Form.wdgButtonThumbLeft->setValue((device->getState().wButtons & XINPUT_GAMEPAD_LEFT_THUMB) != 0);
		m_Form.wdgButtonThumbRight->setValue((device->getState().wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) != 0);

		m_Form.wdgTriggerLeft->setValue((float)device->getState().bLeftTrigger);

		m_Form.wdgTriggerRight->setValue((float)device->getState().bRightTrigger);

		m_Form.wdgStickLeft->setValue(
			(float)device->getState().sThumbLX,
			(float)device->getState().sThumbLY);

		m_Form.wdgStickRight->setValue(
			(float)device->getState().sThumbRX,
			(float)device->getState().sThumbRY);
	}

}; // namespace Pitstop