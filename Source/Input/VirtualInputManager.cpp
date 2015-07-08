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

	bool VirtualInputManager::serialize(QJsonObject& target, size_t version /*= SERIALIZATION_VERSION*/)
	{
		QJsonArray devices_array;
		for (VirtualInputDevicePtr& device : m_Devices)
		{
			QJsonObject device_object;
			if (device->serialize(device_object, version))
			{
				devices_array.push_back(device_object);
			}
		}

		target["devices"] = devices_array;

		return true;
	}

	bool VirtualInputManager::deserialize(const QJsonObject& source, size_t version /*= SERIALIZATION_VERSION*/)
	{
		return true;
	}

}; // namespace Pitstop