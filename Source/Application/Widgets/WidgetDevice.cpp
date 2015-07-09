#include "Application/Widgets/WidgetDevice.h"

#include "Application/Application.h"
#include "Input/Usb/UsbController.h"
#include "Input/RawInputManager.h"

namespace Pitstop {

	WidgetDevice::WidgetDevice(
			RawInputManager& rawInput,
			UsbController& usbController,
			VirtualInputDevicePtr device,
			QWidget* parent /*= nullptr*/,
			Qt::WindowFlags flags /*= 0*/)
		: QWidget(parent, flags)
		, m_RawInput(rawInput)
		, m_UsbController(usbController)
		, m_Device(device)
	{
		connect(
			&m_RawInput, SIGNAL(signalJoystickCreated(RawInputJoystickPtr)),
			this, SLOT(slotJoystickCreated(RawInputJoystickPtr)));

		connect(
			&m_RawInput, SIGNAL(signalJoystickConnected(RawInputJoystickPtr, bool)),
			this, SLOT(slotJoystickConnected(RawInputJoystickPtr, bool)));

		connect(
			device.data(), SIGNAL(signalJoystickChanged(RawInputJoystickPtr)),
			this, SLOT(slotJoystickChanged(RawInputJoystickPtr)));

		connect(
			device.data(), SIGNAL(signalUsbDeviceChanged(UsbDevicePtr)),
			this, SLOT(slotUsbDeviceChanged(UsbDevicePtr)));

		m_Form.setupUi(this);

		updateConnection();
		updateJoysticks();
		updateThumbnail();
	}

	WidgetDevice::~WidgetDevice()
	{
		disconnect(
			this, SLOT(slotUsbDeviceChanged(UsbDevicePtr)));

		disconnect(
			this, SLOT(slotJoystickChanged(RawInputJoystickPtr)));

		disconnect(
			this, SLOT(slotJoystickConnected(RawInputJoystickPtr, bool)));

		disconnect(
			this, SLOT(slotJoystickCreated(RawInputJoystickPtr)));
	}

	void WidgetDevice::slotJoystickCreated(RawInputJoystickPtr joystick)
	{
		updateJoysticks();
	}

	void WidgetDevice::slotJoystickConnected(RawInputJoystickPtr joystick, bool connected)
	{
		updateJoysticks();
	}

	void WidgetDevice::slotJoystickChanged(RawInputJoystickPtr joystick)
	{
		int selected = 0;

		QString joystick_path;
		if (m_Device->getJoystick() != nullptr)
		{
			joystick_path = m_Device->getJoystick()->getUniquePath();
		}

		for (int i = 0; i < m_Form.cmbJoystick->count(); ++i)
		{
			QVariant item_data = m_Form.cmbJoystick->itemData(i);
			if (item_data.isValid())
			{
				if (item_data.toString() == joystick_path)
				{
					selected = i;

					break;
				}
			}
		}

		m_Form.cmbJoystick->setCurrentIndex(selected);
	}

	void WidgetDevice::slotUsbDeviceChanged(UsbDevicePtr usb)
	{
		updateConnection();
	}

	void WidgetDevice::on_cmbJoystick_currentIndexChanged(int index)
	{
		if (m_Device != nullptr)
		{
			RawInputJoystickPtr joystick;

			if (index >= 1 &&
				index < m_Form.cmbJoystick->count())
			{
				QString joystick_path = m_Form.cmbJoystick->currentData().toString();
				joystick = m_RawInput.getJoystickByPath(joystick_path);
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
		if (usb == nullptr)
		{
			usb = m_UsbController.createDevice();
			m_Device->setUsbDevice(usb);
		}

		usb->setConnected(!usb->isConnected());

		updateConnection();
	}

	void WidgetDevice::updateJoysticks()
	{
		int selected = 0;
		int current = 1;

		bool previous = m_Form.cmbJoystick->blockSignals(true);

		m_Form.cmbJoystick->clear();
		m_Form.cmbJoystick->addItem("<None>");

		QVector<RawInputJoystickPtr> joysticks = m_RawInput.getJoysticks();
		for (RawInputJoystickPtr& joystick : joysticks)
		{
			if (joystick->getType() != RawInputJoystick::Type::Raw)
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
				QVariant(joystick->getUniquePath()));

			current++;
		}

		m_Form.cmbJoystick->blockSignals(previous);

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

		bool connected =
			usb != nullptr &&
			usb->isConnected();

		m_Form.btnConnect->setText(connected ? "Disconnect" : "Connect");
	}

}; // namespace Pitstop