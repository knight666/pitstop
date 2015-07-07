#include "Input/VirtualInputManager.h"

#include "Input/RawInputManager.h"

#define MAX_DEVICES 4

namespace Pitstop {

	VirtualInputManager::VirtualInputManager(RawInputManager& rawInput)
		: m_RawInput(rawInput)
	{
	}

	VirtualInputManager::~VirtualInputManager()
	{
		m_Devices.clear();
	}

	VirtualInputDevicePtr VirtualInputManager::createDevice()
	{
		PS_LOG_INFO(VirtualInput) << "Creating device " << m_Devices.size() << ".";

		VirtualInputDevicePtr device(new VirtualInputDevice(*this, m_Devices.size()));

		m_Devices.push_back(device);

		return device;
	}

	VirtualInputDevicePtr VirtualInputManager::getDeviceByIndex(uint8_t index) const
	{
		return (index < m_Devices.size()) ? m_Devices[index] : VirtualInputDevicePtr();
	}

}; // namespace Pitstop