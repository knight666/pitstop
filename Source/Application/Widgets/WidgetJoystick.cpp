#include "Application/Widgets/WidgetJoystick.h"

#include "Input/RawInputJoystick.h"
#include "ui_WidgetJoystick.h"

namespace Pitstop {

	WidgetJoystick::WidgetJoystick(QWidget* parent /*= nullptr*/)
		: QWidget(parent)
		, m_Form(new Ui_JoystickForm)
		, m_Joystick(nullptr)
	{
		m_Form->setupUi(this);
	}

	WidgetJoystick::~WidgetJoystick()
	{
		delete m_Form;
	}

	void WidgetJoystick::bind(RawInputJoystick& joystick)
	{
		m_Form->lblName->setText(joystick.getDescription());
	}

}; // namespace Pitstop