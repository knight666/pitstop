#include "InspectorApplication.h"

#include <Input/Container/ContainerManager.h>
#include <Input/RawInput/RawInputManager.h>

namespace Pitstop {

	InspectorApplication::InspectorApplication(int& argc, char** argv, int flags /*= ApplicationFlags*/)
		: QApplication(argc, argv, flags)
		, m_Containers(new ContainerManager())
		, m_RawInput(new RawInputManager(m_Containers))
	{
		m_MainWindowForm.setupUi(&m_MainWindow);

		connect(
			m_RawInput.data(), SIGNAL(signalJoystickConnected(QSharedPointer<RawInputJoystick>, bool)),
			this, SLOT(slotJoystickConnected(QSharedPointer<RawInputJoystick>, bool)));
	}

	InspectorApplication::~InspectorApplication()
	{
		disconnect(
			this, SLOT(slotJoystickConnected(QSharedPointer<RawInputJoystick>, bool)));
	}

	int InspectorApplication::run()
	{
		if (!m_Containers->initialize())
		{
			PS_LOG_ERROR(ContainerManager) << "Failed to initialize containers.";

			return -1;
		}

		if (!m_RawInput->initialize((HWND)m_MainWindow.winId()))
		{
			PS_LOG_ERROR(RawInputManager) << "Failed to initialize raw input.";

			return -1;
		}

		m_MainWindow.show();

		return exec();
	}

	void InspectorApplication::slotJoystickConnected(QSharedPointer<RawInputJoystick> joystick, bool connected)
	{
		int i = 0;
	}

	bool InspectorApplication::nativeEventFilter(const QByteArray& eventType, void* message, long* result)
	{
		if (eventType != "windows_generic_MSG")
		{
			return false;
		}

		MSG* msg = static_cast<MSG*>(message);

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