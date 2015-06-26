#pragma once

#include "Base/Main.h"

namespace Pitstop {

	class RawInputJoystick;

	class RawInputManager
	{

	public:

		RawInputManager();
		~RawInputManager();

		bool initialize();

	private:

		QVector<RawInputJoystick*> m_Joysticks;

	}; // class RawInputManager

}; // namespace Pitstop