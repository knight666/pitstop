#include "Application/Widgets/WidgetDevice.h"

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
			index--;

			QVector<RawInputJoystickPtr> joysticks = m_RawInput.getJoysticks();
			if (index >= 0 &&
				index < joysticks.size())
			{
				m_Device->setJoystick(joysticks[index]);
			}
			else
			{
				m_Device->setJoystick(RawInputJoystickPtr());
			}
		}

		updateThumbnail();
	}

	void WidgetDevice::updateJoysticks()
	{
		int selected = 0;

		QStringList items;
		items << "<None>";

		QVector<RawInputJoystickPtr> joysticks = m_RawInput.getJoysticks();
		for (RawInputJoystickPtr& joystick : joysticks)
		{
			if (m_Device != nullptr &&
				joystick == m_Device->getJoystick())
			{
				selected = items.size();
			}

			items << joystick->getDescription();
		}

		m_Form.cmbJoystick->clear();
		m_Form.cmbJoystick->addItems(items);
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

}; // namespace Pitstop