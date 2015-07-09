#include "Input/VirtualInputManager.h"

#include "Input/RawInputManager.h"

#define MAX_DEVICES 4

namespace Pitstop {

	VirtualInputManager::VirtualInputManager(QSharedPointer<ConfigurationManager> configuration, RawInputManager& rawInput)
		: ConfigurationBase(configuration, "virtualInput")
		, m_RawInput(rawInput)
	{
	}

	VirtualInputManager::~VirtualInputManager()
	{
		m_Devices.clear();
	}

	VirtualInputDevicePtr VirtualInputManager::createDevice()
	{
		PS_LOG_INFO(VirtualInput) << "Creating device " << m_Devices.size() << ".";

		VirtualInputDevicePtr device(
			new VirtualInputDevice(
				*this,
				m_Configuration,
				m_Devices.size()));

		m_Devices.push_back(device);

		emit signalSaveConfiguration();

		return device;
	}

	VirtualInputDevicePtr VirtualInputManager::getDeviceByIndex(uint8_t index) const
	{
		return (index < m_Devices.size()) ? m_Devices[index] : VirtualInputDevicePtr();
	}

	bool VirtualInputManager::serialize(QJsonObject& target, size_t version)
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

	bool VirtualInputManager::deserialize(const QJsonObject& source, size_t version)
	{
		return true;
	}

}; // namespace Pitstop