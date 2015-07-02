#include "Application/MainWindow.h"

#include <QtWidgets/QVBoxLayout>

#include "Application/Widgets/WidgetJoystick.h"
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
		m_Form.bindingsList->bind(joystick);
	}

	void MainWindow::initialize()
	{
		RawInputJoystickPtr joystick = m_RawInput.getJoystick();
		if (joystick == nullptr)
		{
			return;
		}

		QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(m_Form.tabJoysticks->layout());

		WidgetJoystick* widget = new WidgetJoystick(joystick, m_Form.tabJoysticks);

		layout->addWidget(widget);
	}

}; // namespace Pitstop