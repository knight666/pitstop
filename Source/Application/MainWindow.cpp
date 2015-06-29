#include "Application/MainWindow.h"

#include "Input/RawInputManager.h"

namespace Pitstop {

	MainWindow::MainWindow(RawInputManager& rawInput)
		: QMainWindow(nullptr, 0)
		, m_RawInput(rawInput)
	{
		m_Form.setupUi(this);
	}

	MainWindow::~MainWindow()
	{
	}

	void MainWindow::bindJoystick(RawInputJoystick& joystick)
	{
		if (joystick.getInputProcessor() == nullptr)
		{
			return;
		}

		m_Form.bindingsList->bind(*joystick.getInputProcessor());
	}

	void MainWindow::updateBindings()
	{
		m_Form.bindingsList->update();
	}

}; // namespace Pitstop