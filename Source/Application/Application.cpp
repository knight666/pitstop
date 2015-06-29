#include "Application/Application.h"

#include <QtGui/QWindow>
#include <QtGui/QGuiApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

#include "Application/MainWindow.h"
#include "Input/Process/InputProcessorDualShock4.h"
#include "Input/Process/InputProcessorFFBWheel.h"
#include "Input/Usb/UsbController.h"
#include "Input/RawInputManager.h"

namespace Pitstop {

	Application::Application(int& argc, char** argv, int flags /*= ApplicationFlags*/)
		: QApplication(argc, argv, flags)
		, m_RawInput(new RawInputManager())
		, m_UsbController(new UsbController())
		, m_MainWindow(new MainWindow(*m_RawInput))
	{
		installNativeEventFilter(this);

		m_RawInput->registerInputProcessor<InputProcessorDualShock4>();
		m_RawInput->registerInputProcessor<InputProcessorFFBWheel>();
	}

	Application::~Application()
	{
		delete m_UsbController;
		delete m_RawInput;
		delete m_MainWindow;
	}

	int Application::run()
	{
		if (!m_RawInput->initialize((HWND)m_MainWindow->winId()) ||
			!m_UsbController->initialize())
		{
			return false;
		}

		RawInputJoystick* joystick = m_RawInput->getJoystick();
		if (joystick != nullptr)
		{
			m_MainWindow->bindJoystick(*joystick);
		}
		m_MainWindow->show();

		return exec();
	}

	bool Application::nativeEventFilter(const QByteArray& eventType, void* message, long* result)
	{
		MSG* msg = (MSG*)message;

		if (eventType == "windows_generic_MSG" &&
			msg->message == WM_INPUT)
		{
			m_RawInput->processInputMessage(msg->wParam, msg->lParam);

			m_MainWindow->updateBindings();

			return true;
		}

		return false;
	}

}; // namespace Pitstop