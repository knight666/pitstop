#include "Input/XInput/XInputManager.h"

namespace Pitstop {

	static const qint64 s_RetryTime = 30000;

	XInputManager::XInputManager()
		: m_Library(NULL)
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
		m_Devices.clear();

		if (m_Library != NULL)
		{
			::FreeLibrary(m_Library);
			m_Library = NULL;
		}
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

		PS_LOG_INFO(XInputManager) << "Start XInput thread.";

		start();

		return true;
	}

	void XInputManager::run()
	{
		while (1)
		{
			updateGamepadState();
		}
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
					activated)
				{
					PS_LOG_TRACE(XInputManager) << "Gamepad " << user << " connected " << connected << " activated " << activated << ".";

					device->updateState(xinput_state, connected);
				}

				device->setLastCheck(current);
			}
		}
	}

}; // namespace Pitstop