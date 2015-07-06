#include "Application/MainWindow.h"

#include <QtWidgets/QVBoxLayout>

#include "Input/RawInputManager.h"

namespace Pitstop {

	MainWindow::MainWindow(RawInputManager& rawInput)
		: QMainWindow(nullptr, 0)
		, m_RawInput(rawInput)
	{
		m_Form.setupUi(this);

		m_Form.scrollAreaWidgetContents->setLayout(new QVBoxLayout(m_Form.scrollAreaWidgetContents));

		connect(
			&m_RawInput, SIGNAL(signalJoystickConnected(RawInputJoystickPtr, bool)),
			this, SLOT(slotJoystickConnected(RawInputJoystickPtr, bool)));
	}

	MainWindow::~MainWindow()
	{
		disconnect(
			this, SLOT(slotJoystickConnected(RawInputJoystickPtr, bool)));
	}

	void MainWindow::bindJoystick(RawInputJoystick& joystick)
	{
		m_Form.bindingsList->bind(joystick);
	}

	void MainWindow::slotJoystickConnected(RawInputJoystickPtr joystick, bool connected)
	{
		QHash<QString, WidgetJoystickPtr>::iterator found = m_JoystickWidgets.find(joystick->getDevicePath());
		if (found != m_JoystickWidgets.end())
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

		m_JoystickWidgets.insert(joystick->getDevicePath(), widget);
	}

}; // namespace Pitstop