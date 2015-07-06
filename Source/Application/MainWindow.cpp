#include "Application/MainWindow.h"

#include "Input/RawInputManager.h"

namespace Pitstop {

	MainWindow::MainWindow(RawInputManager& rawInput)
		: QMainWindow(nullptr, 0)
		, m_RawInput(rawInput)
	{
		m_Form.setupUi(this);

		m_Form.tabJoysticks->setup(rawInput);
		m_Form.tabDevices->setup(rawInput);
	}

	MainWindow::~MainWindow()
	{
	}

	void MainWindow::bindJoystick(RawInputJoystick& joystick)
	{
		m_Form.bindingsList->bind(joystick);
	}

}; // namespace Pitstop