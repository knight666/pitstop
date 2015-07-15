#pragma once

#include "Base/Main.h"

namespace Pitstop {

	class XInputManager
		: public QThread
	{

		Q_OBJECT

	public:

		XInputManager();
		~XInputManager();

		bool initialize();

		void updateGamepadState(bool forceUpdate = false);

	signals:

		void signalGamepadChanged(DWORD index, bool connected, bool activated);

	private:

		virtual void run() override;

	private:

		QElapsedTimer m_Elapsed;

		HMODULE m_Library;
		typedef DWORD (WINAPI* XInputGetStateFunc)(DWORD, XINPUT_STATE*);
		XInputGetStateFunc m_LibraryXInputGetState;

		struct GamepadState
		{
			bool connected;
			qint64 last_check;
			DWORD packet;
		};
		GamepadState m_Gamepads[XUSER_MAX_COUNT];

	};

}; // namespace Pitstop