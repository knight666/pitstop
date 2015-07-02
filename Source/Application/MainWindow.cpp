#include "Application/MainWindow.h"

#include "Input/RawInputManager.h"

namespace Pitstop {

	MainWindow::MainWindow(RawInputManager& rawInput)
		: QMainWindow(nullptr, 0)
		, m_RawInput(rawInput)
	{
		m_Form.setupUi(this);

		connect(
			&m_RawInput, SIGNAL(signalJoystickInput(RawInputJoystick*)),
			this, SLOT(slotJoystickInput(RawInputJoystick*)));
	}

	MainWindow::~MainWindow()
	{
		disconnect(
			this, SLOT(slotJoystickInput(RawInputJoystick*)));
	}

	void MainWindow::bindJoystick(RawInputJoystick& joystick)
	{
		m_Form.bindingsList->bind(joystick);
	}

	void MainWindow::updateBindings()
	{
		m_Form.bindingsList->update();
	}

	void MainWindow::slotJoystickInput(RawInputJoystick* joystick)
	{

	}

}; // namespace Pitstop