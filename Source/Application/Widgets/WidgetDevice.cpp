#include "Application/Widgets/WidgetDevice.h"

namespace Pitstop {

	WidgetDevice::WidgetDevice(VirtualInputDevicePtr device, QWidget* parent /*= nullptr*/, Qt::WindowFlags flags /*= 0*/)
		: QWidget(parent, flags)
		, m_Device(device)
	{
		m_Form.setupUi(this);

		setupDevice(device);
	}

	WidgetDevice::~WidgetDevice()
	{
	}

	void WidgetDevice::setupDevice(VirtualInputDevicePtr device)
	{
		RawInputJoystickPtr joystick = device->getJoystick();
		if (joystick != nullptr)
		{
			QSharedPointer<QImage> thumbnail = joystick->getThumbnail();

			if (thumbnail != nullptr &&
				!thumbnail.isNull())
			{
				m_Form.lblImage->setPixmap(QPixmap::fromImage(*thumbnail));
				m_Form.lblImage->adjustSize();
			}
		}

		m_Device = device;
	}

}; // namespace Pitstop