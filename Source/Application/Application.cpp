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
		if (!m_RawInput->initialize((HWND)m_MainWindow->winId()) ||
			!m_UsbController->initialize())
		{
			return false;
		}

		QVBoxLayout* layout = m_MainWindow->findChild<QVBoxLayout*>("labelsContainer");

		RawInputJoystick* joystick = m_RawInput->getJoystick();
		if (joystick != nullptr)
		{
			InputProcessorBase* processor = joystick->getInputProcessor();
			if (processor != nullptr)
			{
				const QHash<QString, InputProcessorBase::InputBinding>& bindings = processor->getBindings();
				for (QHash<QString, InputProcessorBase::InputBinding>::const_iterator it = bindings.begin(); it != bindings.end(); ++it)
				{
					const InputProcessorBase::InputBinding& binding = it.value();
					layout->addWidget(new QLabel(QString("%1: %2").arg(it.key()).arg(binding.digitalValue)));
				}
			}
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

			return true;
		}

		return false;
	}

}; // namespace Pitstop