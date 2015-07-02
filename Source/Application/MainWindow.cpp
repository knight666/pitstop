#include "Application/MainWindow.h"

#include "Input/RawInputJoystick.h"

namespace Pitstop {

	MainWindow::MainWindow()
		: QMainWindow(nullptr, 0)
	{
		m_Form.setupUi(this);
	}

	MainWindow::~MainWindow()
	{
	}

	void MainWindow::bindJoystick(RawInputJoystick& joystick)
	{
		m_Form.bindingsList->bind(joystick);
	}

}; // namespace Pitstop