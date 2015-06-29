#include "Application/Application.h"

#include <QtGui/QWindow>
#include <QtGui/QGuiApplication>

#include "Application/MainWindow.h"
#include "Input/Process/InputProcessorDualShock4.h"
#include "Input/Usb/UsbController.h"
#include "Input/RawInputManager.h"

namespace Pitstop {

	Application::Application(int& argc, char** argv, int flags /*= ApplicationFlags*/)
		: QApplication(argc, argv, flags)
		, m_MainWindow(new MainWindow())
		, m_RawInput(new RawInputManager())
		, m_UsbController(new UsbController())
	{
		installNativeEventFilter(this);

		m_RawInput->registerInputProcessor<InputProcessorDualShock4>();
	}

	Application::~Application()
	{
		delete m_UsbController;
		delete m_RawInput;
		delete m_MainWindow;
	}

	int Application::run()
	{
		m_MainWindow->show();

		if (!m_RawInput->initialize((HWND)m_MainWindow->winId()) ||
			!m_UsbController->initialize())
		{
			return false;
		}

		return exec();
	}

	bool Application::nativeEventFilter(const QByteArray& eventType, void* message, long* result)
	{
		MSG* msg = (MSG*)message;

		if (eventType == "windows_generic_MSG" &&
			msg->message == WM_INPUT)
		{
			m_RawInput->processInputMessage(msg->wParam, msg->lParam);

			return true;
		}

		return false;
	}

}; // namespace Pitstop