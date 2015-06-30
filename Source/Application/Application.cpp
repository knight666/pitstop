#include "Application/Application.h"

#include <QtGui/QWindow>
#include <QtGui/QGuiApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

#include "Application/MainWindow.h"
#include "Input/Process/InputProcessorDualShock4.h"
#include "Input/Usb/UsbController.h"
#include "Input/RawInputManager.h"
#include "Input/VirtualInputManager.h"

namespace Pitstop {

	Application::Application(int& argc, char** argv, int flags /*= ApplicationFlags*/)
		: QApplication(argc, argv, flags)
		, m_RawInput(new RawInputManager())
		, m_UsbController(new UsbController())
		, m_VirtualInput(new VirtualInputManager())
		, m_MainWindow(new MainWindow(*m_RawInput))
	{
		installNativeEventFilter(this);

		m_RawInput->registerInputProcessor<InputProcessorDualShock4>();
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

		RawInputJoystick* joystick = m_RawInput->getJoystick();
		if (joystick != nullptr)
		{
			m_VirtualInput->bindJoystick(0, *joystick);
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
			UINT raw_size = 0;
			if (::GetRawInputData(
				(HRAWINPUT)msg->lParam,
				RID_INPUT,
				NULL,
				&raw_size,
				sizeof(RAWINPUTHEADER)) != (UINT)-1)
			{
				QVector<uint8_t> raw_data;
				raw_data.resize(raw_size);
				RAWINPUT* raw_input = (RAWINPUT*)&raw_data[0];

				if (::GetRawInputData(
					(HRAWINPUT)msg->lParam,
					RID_INPUT,
					raw_input,
					&raw_size,
					sizeof(RAWINPUTHEADER)) == (UINT)-1 ||
					raw_input->header.dwType == RIM_TYPEHID)
				{
					m_RawInput->processInputMessage(*raw_input, raw_input->header.hDevice);

					m_VirtualInput->update(raw_input->header.hDevice);

					m_MainWindow->updateBindings();
				}
			}

			return true;
		}

		return false;
	}

}; // namespace Pitstop