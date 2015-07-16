#pragma once

#include "Base/Main.h"

#include "Input/XInputState.h"

namespace Pitstop {

	class RawInputJoystick;

	class XInputDevice
		: public QObject
	{

		Q_OBJECT

	public:

		XInputDevice(DWORD userIndex);
		~XInputDevice();

		DWORD getUserIndex() const { return m_UserIndex; }

		const XINPUT_GAMEPAD& getState() { return m_State; }

		bool isConnected() const { return m_Connected; }
		void setConnected(bool value) { m_Connected = value; }

		bool isActivated() const { return m_Activated; }

		DWORD getLastPacket() const { return m_LastPacket; }

		qint64 getLastCheck() const { return m_LastCheck; }
		void setLastCheck(qint64 value) { m_LastCheck = value; }

		void updateState(XINPUT_STATE& state, bool connected);

	signals:

		void signalChanged();

	private:

		DWORD m_UserIndex;
		XINPUT_GAMEPAD m_State;
		bool m_Connected;
		bool m_Activated;
		DWORD m_LastPacket;
		qint64 m_LastCheck;

	}; // class XInputDevice

}; // namespace Pitstop