#include "Input/VirtualInputManager.h"

#include "Input/VirtualInputDevice.h"
#include "Input/RawInputJoystick.h"

#define MAX_DEVICES 4

namespace Pitstop {

	VirtualInputManager::VirtualInputManager()
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

	void VirtualInputManager::bindJoystick(uint8_t index, RawInputJoystick& joystick)
	{
		if (index >= MAX_DEVICES)
		{
			return;
		}

		VirtualInputDevice* device = m_Devices[index];
		device->setJoystick(joystick);
		m_RawInputMapping[joystick.getHandle()] = device;
	}

	void VirtualInputManager::update(HANDLE device)
	{
		QHash<HANDLE, VirtualInputDevice*>::iterator found = m_RawInputMapping.find(device);
		if (found != m_RawInputMapping.end())
		{
			found.value()->update();
		}
	}

}; // namespace Pitstop