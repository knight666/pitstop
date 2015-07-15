#pragma once

#include "Base/Main.h"
#include "Input/XInput/XInputDevice.h"

namespace Pitstop {

	class XInputManager
		: public QThread
	{

		Q_OBJECT

	public:

		XInputManager();
		~XInputManager();

		QSharedPointer<XInputDevice> getDeviceByIndex(size_t index) const;

		bool initialize();

		void updateGamepadState(bool forceUpdate = false);

	private:

		virtual void run() override;

	private:

		QElapsedTimer m_Elapsed;

		HMODULE m_Library;
		typedef DWORD (WINAPI* XInputGetStateFunc)(DWORD, XINPUT_STATE*);
		XInputGetStateFunc m_LibraryXInputGetState;

		QVector<QSharedPointer<XInputDevice>> m_Devices;

	};

}; // namespace Pitstop