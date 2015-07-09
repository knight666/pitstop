#include "Application/Widgets/WidgetDevice.h"

#include "Application/Application.h"
#include "Input/RawInputManager.h"

namespace Pitstop {

	WidgetDevice::WidgetDevice(RawInputManager& rawInput, VirtualInputDevicePtr device, QWidget* parent /*= nullptr*/, Qt::WindowFlags flags /*= 0*/)
		: QWidget(parent, flags)
		, m_RawInput(rawInput)
		, m_Device(device)
	{
		connect(
			&m_RawInput, SIGNAL(signalJoystickConnected(RawInputJoystickPtr, bool)),
			this, SLOT(slotJoystickConnected(RawInputJoystickPtr, bool)));

		m_Form.setupUi(this);

		updateConnection();
		updateJoysticks();
		updateThumbnail();
	}

	WidgetDevice::~WidgetDevice()
	{
		disconnect(
			this, SLOT(slotJoystickConnected(RawInputJoystickPtr, bool)));
	}

	void WidgetDevice::slotJoystickConnected(RawInputJoystickPtr joystick, bool connected)
	{
		updateJoysticks();
	}

	void WidgetDevice::on_cmbJoystick_currentIndexChanged(int index)
	{
		if (m_Device != nullptr)
		{
			RawInputJoystickPtr joystick;

			if (index >= 1 &&
				index < m_Form.cmbJoystick->count())
			{
				QVariant joystick_handle = m_Form.cmbJoystick->currentData(index);
				joystick = m_RawInput.getJoystickByHandle((HANDLE)joystick_handle.toUInt());
			}

			m_Device->setJoystick(joystick);
		}

		updateThumbnail();
	}

	void WidgetDevice::on_btnConnect_pressed()
	{
		if (m_Device == nullptr)
		{
			return;
		}

		UsbDevicePtr usb = m_Device->getUsbDevice();
		if (usb != nullptr)
		{
			usb->setPluggedIn(!usb->isPluggedIn());
		}

		updateConnection();
	}

	void WidgetDevice::updateJoysticks()
	{
		int current = 0;
		int selected = 0;

		m_Form.cmbJoystick->clear();
		m_Form.cmbJoystick->addItem("<None>");

		QVector<RawInputJoystickPtr> joysticks = m_RawInput.getJoysticks();
		for (RawInputJoystickPtr& joystick : joysticks)
		{
			if (!joystick->isConnected() ||
				joystick->getType() != RawInputJoystick::Type::Raw)
			{
				continue;
			}

			if (m_Device != nullptr &&
				joystick == m_Device->getJoystick())
			{
				selected = current;
			}

			m_Form.cmbJoystick->addItem(
				joystick->getDescription(),
				QVariant((uint)joystick->getHandle()));

			current++;
		}

		m_Form.cmbJoystick->setCurrentIndex(selected);
	}

	void WidgetDevice::updateThumbnail()
	{
		QPixmap thumbnail_image;

		if (m_Device != nullptr &&
			m_Device->getJoystick() != nullptr)
		{
			QSharedPointer<QImage> thumbnail = m_Device->getJoystick()->getThumbnail();

			if (thumbnail != nullptr &&
				!thumbnail.isNull())
			{
				thumbnail_image = QPixmap::fromImage(*thumbnail);
			}
		}

		m_Form.lblImage->setPixmap(thumbnail_image);
		m_Form.lblImage->adjustSize();
	}

	void WidgetDevice::updateConnection()
	{
		UsbDevicePtr usb;

		if (m_Device != nullptr)
		{
			usb = m_Device->getUsbDevice();
		}

		if (usb != nullptr)
		{
			m_Form.btnConnect->setText(usb->isPluggedIn() ? "Disconnect" : "Connect");
		}
		m_Form.btnConnect->setEnabled(usb != nullptr);
	}

}; // namespace Pitstop