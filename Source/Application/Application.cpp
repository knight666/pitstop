#include "Application/Application.h"

#include "Application/MainWindow.h"

namespace Pitstop {

	Application::Application(int& argc, char** argv, int flags /*= ApplicationFlags*/)
		: QApplication(argc, argv, flags)
	{
		m_RawInput = new RawInputManager();

		m_MainWindow = new MainWindow();
	}

	Application::~Application()
	{
		delete m_MainWindow;
		delete m_RawInput;
	}

	int Application::run()
	{
		m_MainWindow->show();

		if (!m_RawInput->initialize())
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