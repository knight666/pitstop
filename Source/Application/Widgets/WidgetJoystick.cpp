#include "Application/Widgets/WidgetJoystick.h"

namespace Pitstop {

	WidgetJoystick::WidgetJoystick(RawInputJoystickPtr joystick, QWidget* parent /*= nullptr*/)
		: QWidget(parent)
		, m_Joystick(joystick)
	{
		m_Form.setupUi(this);

		connect(
			m_Joystick.data(), SIGNAL(signalConnected(RawInputJoystick&, bool)),
			this, SLOT(slotJoystickConnected(RawInputJoystick&, bool)));

		slotJoystickConnected(*joystick, joystick->isConnected());
	}

	WidgetJoystick::~WidgetJoystick()
	{
		disconnect(
			this, SLOT(slotJoystickConnected(RawInputJoystick&, bool)));
	}

	void WidgetJoystick::slotJoystickConnected(RawInputJoystick& joystick, bool connected)
	{
		if (m_Joystick != &joystick)
		{
			return;
		}

		// Thumbnail

		QSharedPointer<QImage> thumbnail = joystick.getThumbnail();

		if (thumbnail != nullptr &&
			!thumbnail.isNull())
		{
			m_Form.lblImage->setPixmap(QPixmap::fromImage(*thumbnail));
			m_Form.lblImage->adjustSize();
		}

		// Name

		m_Form.lblName->setText(
			QString("%1")
				.arg(m_Joystick->getDescription()));

		// Link

		m_Form.btnLink->setDisabled(joystick.getType() == RawInputJoystick::Type::XInput);

		// XInput

		QString icon_resource = ":/Icons/Resources/";

		if (connected)
		{
			icon_resource += "Connected-50.png";
		}
		else
		{
			icon_resource += "Disconnected-50.png";
		}

		m_Form.icoConnected->setPixmap(QPixmap(icon_resource));

		m_Form.lblXinput->setText(
			QString("%1")
				.arg(m_Joystick->getXinputIndex()));
	}

}; // namespace Pitstop