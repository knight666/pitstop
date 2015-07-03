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

		m_Form.lblName->setText(
			QString("%1 [%2] (%3)")
				.arg(m_Joystick->getDescription())
				.arg(m_Joystick->getXinputIndex())
				.arg(connected ? "CONNECTED" : "DISCONNECTED"));

		m_Form.btnLink->setDisabled(joystick.getType() == RawInputJoystick::Type::XInput);
	}

}; // namespace Pitstop