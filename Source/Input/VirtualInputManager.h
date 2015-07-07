#pragma once

#include "Base/Main.h"

#include "Input/VirtualInputDevice.h"

namespace Pitstop {

	class RawInputManager;

	class VirtualInputManager
		: public QObject
	{

		Q_OBJECT

	public:

		VirtualInputManager(RawInputManager& rawInput);
		~VirtualInputManager();

		VirtualInputDevicePtr createDevice();

		VirtualInputDevicePtr getDeviceByIndex(uint8_t index) const;

	private:

		RawInputManager& m_RawInput;
		QVector<VirtualInputDevicePtr> m_Devices;

	}; // class VirtualInputManager

}; // namespace Pitstop