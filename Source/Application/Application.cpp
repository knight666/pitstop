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

namespace Pitstop {

	Application::Application(int& argc, char** argv, int flags /*= ApplicationFlags*/)
		: QApplication(argc, argv, flags)
		, m_RawInput(new RawInputManager())
		, m_UsbController(new UsbController())
		, m_VirtualInput(new VirtualInputManager(*m_RawInput))
		, m_MainWindow(new MainWindow(*m_RawInput))
	{
		installNativeEventFilter(this);

		m_RawInput->registerInputProcessor<InputProcessorDualShock4>();
		m_RawInput->registerInputProcessor<InputProcessorFFBWheel>();
	}

	Application::~Application()
	{
		delete m_VirtualInput;
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

		m_MainWindow->initialize();

		VirtualInputDevice* device = m_VirtualInput->getDeviceByIndex(0);

		RawInputJoystickPtr joystick = m_RawInput->getJoystick();
		if (joystick != nullptr)
		{
			device->setJoystick(*joystick);

			UsbDevice* usb = m_UsbController->getDeviceByIndex(0);
			usb->setPluggedIn(true);

			device->setUsbDevice(*usb);

			m_MainWindow->bindJoystick(*joystick);
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