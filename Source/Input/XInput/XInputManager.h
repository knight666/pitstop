#pragma once

#include "Base/Main.h"
#include "Input/XInput/XInputDevice.h"

namespace Pitstop {

	class RawInputJoystick;
	class RawInputManager;

	class XInputManager
		: public QThread
	{

		Q_OBJECT

	public:

		XInputManager(RawInputManager& rawInput);
		~XInputManager();

		QSharedPointer<XInputDevice> getDeviceByIndex(size_t index) const;

		bool initialize();

		void updateGamepadState(bool forceUpdate = false);

	public slots:

		void slotJoystickConnected(QSharedPointer<RawInputJoystick> joystick, bool connected);

	private:

		virtual void run() override;

	private:

		RawInputManager& m_RawInput;

		QElapsedTimer m_Elapsed;

		HMODULE m_Library;
		typedef DWORD (WINAPI* XInputGetStateFunc)(DWORD, XINPUT_STATE*);
		XInputGetStateFunc m_LibraryXInputGetState;

		QVector<QSharedPointer<XInputDevice>> m_Devices;

	};

}; // namespace Pitstop