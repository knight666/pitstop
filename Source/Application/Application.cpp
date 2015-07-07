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
#include "Input/VirtualInputManager.h"
#include "Logging/SinkFile.h"

namespace Pitstop {

	Application::Application(int& argc, char** argv, int flags /*= ApplicationFlags*/)
		: QApplication(argc, argv, flags)
		, m_RawInput(new RawInputManager())
		, m_UsbController(new UsbController())
		, m_VirtualInput(new VirtualInputManager(*m_RawInput))
		, m_MainWindow(new MainWindow(*m_RawInput, *m_UsbController, *m_VirtualInput))
	{
		setApplicationName("Pitstop");

		Logger::initialize();

		PS_LOG_INFO(Application) << "Initializing application.";

		installNativeEventFilter(this);

		m_RawInput->registerInputProcessor<InputProcessorDualShock4>();
		m_RawInput->registerInputProcessor<InputProcessorFFBWheel>();
	}

	Application::~Application()
	{
		PS_LOG_INFO(Application) << "Closing application.";

		delete m_VirtualInput;
		delete m_UsbController;
		delete m_RawInput;
		delete m_MainWindow;

		Logger::destroy();
	}

	int Application::run()
	{
		if (!m_RawInput->initialize((HWND)m_MainWindow->winId()))
		{
			PS_LOG_ERROR(RawInput) << "Failed to initialize raw input.";

			return false;
		}

		if (!m_UsbController->initialize())
		{
			PS_LOG_ERROR(UsbController) << "Failed to initialize virtual USB hub.";

			return false;
		}

		m_MainWindow->show();

		return exec();
	}

	bool Application::nativeEventFilter(const QByteArray& eventType, void* message, long* result)
	{
		if (eventType != "windows_generic_MSG")
		{
			return false;
		}

		MSG* msg = (MSG*)message;

		switch (msg->message)
		{

		case WM_INPUT:
			m_RawInput->processInput(msg->lParam, msg->wParam);
			return true;

		case WM_INPUT_DEVICE_CHANGE:
			m_RawInput->processConnectionChanged(msg->lParam, msg->wParam);
			return true;

		default:
			return false;

		}
	}

}; // namespace Pitstop