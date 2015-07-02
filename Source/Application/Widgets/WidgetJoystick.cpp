#include "Application/Widgets/WidgetJoystick.h"

namespace Pitstop {

	WidgetJoystick::WidgetJoystick(RawInputJoystickPtr joystick, QWidget* parent /*= nullptr*/)
		: QWidget(parent)
		, m_Joystick(joystick)
	{
		m_Form.setupUi(this);

		m_Form.lblName->setText(joystick->getDescription());
	}

	WidgetJoystick::~WidgetJoystick()
	{
	}

}; // namespace Pitstop