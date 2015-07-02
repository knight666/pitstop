#include "Input/VirtualInputManager.h"

#include "Input/RawInputManager.h"

#define MAX_DEVICES 4

namespace Pitstop {

	VirtualInputManager::VirtualInputManager(RawInputManager& rawInput)
		: m_RawInput(rawInput)
	{
		for (uint8_t i = 0; i < MAX_DEVICES; ++i)
		{
			m_Devices.push_back(new VirtualInputDevice(*this, i));
		}
	}

	VirtualInputManager::~VirtualInputManager()
	{
		qDeleteAll(m_Devices);
	}

	VirtualInputDevice* VirtualInputManager::getDeviceByIndex(uint8_t index) const
	{
		return (index < m_Devices.size()) ? m_Devices[index] : nullptr;
	}

	VirtualInputDevice* VirtualInputManager::getDeviceByHandle(HANDLE handle) const
	{
		for (VirtualInputDevice* device : m_Devices)
		{
			if (device->getJoystickHandle() == device)
			{
				return device;
			}
		}

		return nullptr;
	}

}; // namespace Pitstop