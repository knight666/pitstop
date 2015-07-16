#include "Input/XInput/XInputManager.h"

#include "Input/RawInput/RawInputManager.h"

namespace Pitstop {

	static const qint64 s_RetryTime = 30000;

	XInputManager::XInputManager(RawInputManager& rawInput)
		: m_RawInput(rawInput)
		, m_Running(false)
		, m_Library(NULL)
	{
		for (DWORD user = 0; user < XUSER_MAX_COUNT; ++user)
		{
			QSharedPointer<XInputDevice> device(
				new XInputDevice(user));

			m_Devices.push_back(device);
		}
	}

	XInputManager::~XInputManager()
	{
		stop();

		disconnect(
			this, SLOT(slotJoystickConnected(QSharedPointer<RawInputJoystick>, bool)));

		m_Devices.clear();

		if (m_Library != NULL)
		{
			::FreeLibrary(m_Library);
			m_Library = NULL;
		}
	}

	QSharedPointer<XInputDevice> XInputManager::getDeviceByIndex(size_t index) const
	{
		if (index >= (size_t)m_Devices.size())
		{
			return QSharedPointer<XInputDevice>();
		}

		return m_Devices[index];
	}

	bool XInputManager::initialize()
	{
		PS_LOG_INFO(XInputManager) << "Initializing XInput.";

		m_Library = ::LoadLibraryW(XINPUT_DLL_W);
		if (m_Library == NULL)
		{
			// Try to load the Vista-compatible DLL instead.
			m_Library = ::LoadLibraryW(L"xinput9_1_0.dll");
		}

		if (m_Library == NULL)
		{
			PS_LOG_ERROR(XInputManager) << "Failed to load XInput library dynamically.";

			return false;
		}

		m_LibraryXInputGetState = (XInputGetStateFunc)::GetProcAddress(m_Library, "XInputGetState");
		if (m_LibraryXInputGetState == NULL)
		{
			PS_LOG_ERROR(XInputManager) << "Failed to retrieve \"XInputGetState\" from XInput library.";

			return false;
		}

		m_Elapsed.start();

		updateGamepadState(true);

		connect(
			&m_RawInput, SIGNAL(signalJoystickConnected(QSharedPointer<RawInputJoystick>, bool)),
			this, SLOT(slotJoystickConnected(QSharedPointer<RawInputJoystick>, bool)));

		PS_LOG_INFO(XInputManager) << "Starting thread.";

		start();

		return true;
	}

	void XInputManager::stop()
	{
		QMutexLocker lock(&m_RunningLock);
		if (m_Running)
		{
			PS_LOG_INFO(XInputManager) << "Stopping thread.";
			m_Running = false;
		}
	}

	void XInputManager::slotJoystickConnected(QSharedPointer<RawInputJoystick> joystick, bool connected)
	{
		updateGamepadState(true);
	}

	void XInputManager::run()
	{
		{
			QMutexLocker lock(&m_RunningLock);
			m_Running = true;
		}

		PS_LOG_INFO(XInputManager) << "Thread is running.";

		while (isRunning())
		{
			updateGamepadState();
		}

		PS_LOG_INFO(XInputManager) << "Thread was stopped.";
	}

	bool XInputManager::isRunning()
	{
		QMutexLocker lock(&m_RunningLock);
		return m_Running;
	}

	void XInputManager::updateGamepadState(bool forceUpdate /*= false*/)
	{
		qint64 current = m_Elapsed.elapsed();

		XINPUT_STATE xinput_state = { 0 };

		for (DWORD user = 0; user < XUSER_MAX_COUNT; ++user)
		{
			QSharedPointer<XInputDevice>& device = m_Devices[user];

			if (forceUpdate ||
				device->isConnected() ||
				(current - device->getLastCheck()) > s_RetryTime)
			{
				bool connected = (m_LibraryXInputGetState(user, &xinput_state) == ERROR_SUCCESS);
				bool activated = device->getLastPacket() != xinput_state.dwPacketNumber;

				if (connected != device->isConnected() ||
					activated ||
					device->isActivated())
				{
					PS_LOG_TRACE(XInputManager) << "Gamepad " << user << " connected " << connected << " activated " << activated << ".";

					device->updateState(xinput_state, connected);
				}

				device->setLastCheck(current);
			}
		}
	}

}; // namespace Pitstop