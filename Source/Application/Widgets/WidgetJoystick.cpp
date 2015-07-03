#include "Application/Widgets/WidgetJoystick.h"

namespace Pitstop {

	WidgetJoystick::WidgetJoystick(RawInputJoystickPtr joystick, QWidget* parent /*= nullptr*/)
		: QWidget(parent)
		, m_Joystick(joystick)
	{
		m_Form.setupUi(this);

		m_Form.lblName->setText(joystick->getDescription());

		connect(
			m_Joystick.data(), SIGNAL(signalConnected(RawInputJoystick&, bool)),
			this, SLOT(slotJoystickConnected(RawInputJoystick&, bool)));
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
			QString("%1 (%2)")
				.arg(m_Joystick->getDescription())
				.arg(connected ? "CONNECTED" : "DISCONNECTED")
		);
	}

}; // namespace Pitstop