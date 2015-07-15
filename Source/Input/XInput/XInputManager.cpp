#include "Input/XInput/XInputManager.h"

namespace Pitstop {

	static const qint64 s_RetryTime = 30000;

	XInputManager::XInputManager()
		: m_Library(NULL)
	{
		memset(&m_Gamepads, 0, sizeof(m_Gamepads));
	}

	XInputManager::~XInputManager()
	{
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
			GamepadState& gamepad = m_Gamepads[user];

			if (forceUpdate ||
				gamepad.connected ||
				(current - gamepad.last_check) > s_RetryTime)
			{
				bool connected = (m_LibraryXInputGetState(user, &xinput_state) == ERROR_SUCCESS);
				bool activated = gamepad.packet != xinput_state.dwPacketNumber;

				if (connected != gamepad.connected ||
					activated)
				{
					PS_LOG_TRACE(XInputManager) << "Gamepad " << user << " connected " << connected << " activated " << activated << ".";

					emit signalGamepadChanged(user, connected, activated);

					gamepad.connected = connected;
					gamepad.packet = xinput_state.dwPacketNumber;
				}

				gamepad.last_check = current;
			}
		}
	}

}; // namespace Pitstop