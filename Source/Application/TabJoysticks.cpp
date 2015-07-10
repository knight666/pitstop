#include "Application/TabJoysticks.h"

#include <QtWidgets/QVBoxLayout>

#include "Input/RawInputManager.h"

namespace Pitstop {

	TabJoysticks::TabJoysticks(QWidget* parent /*= nullptr*/, Qt::WindowFlags flags /*= 0*/)
		: QWidget(parent, flags)
	{
		m_Form.setupUi(this);

		m_Form.scrollAreaWidgetContents->setLayout(new QVBoxLayout(m_Form.scrollAreaWidgetContents));
	}

	TabJoysticks::~TabJoysticks()
	{
		disconnect(
			this, SLOT(slotJoystickCreated(RawInputJoystickPtr, bool)));
	}

	void TabJoysticks::setup(RawInputManager& rawInput)
	{
		connect(
			&rawInput, SIGNAL(signalJoystickCreated(RawInputJoystickPtr)),
			this, SLOT(slotJoystickCreated(RawInputJoystickPtr)));
	}

	void TabJoysticks::slotJoystickCreated(RawInputJoystickPtr joystick)
	{
		QHash<QString, WidgetJoystickPtr>::iterator found = m_Joysticks.find(joystick->getUniquePath());
		if (found != m_Joysticks.end())
		{
			return;
		}

		WidgetJoystickPtr widget(
			new WidgetJoystick(joystick, m_Form.scrollAreaWidgetContents));

		QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(m_Form.scrollAreaWidgetContents->layout());
		if (layout != nullptr)
		{
			layout->addWidget(widget.data());
		}

		m_Joysticks.insert(joystick->getUniquePath(), widget);
	}

}; // namespace Pitstop