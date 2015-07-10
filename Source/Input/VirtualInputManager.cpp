#include "Input/VirtualInputManager.h"

#include "Input/RawInputManager.h"
#include "Usb/UsbController.h"

namespace Pitstop {

	VirtualInputManager::VirtualInputManager(
			QSharedPointer<ConfigurationManager> configuration,
			RawInputManager& rawInput,
			UsbController& usbController)
		: ConfigurationBase(configuration, "virtualInput")
		, m_RawInput(rawInput)
		, m_UsbController(usbController)
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
				m_Configuration,
				*this,
				m_Devices.size()));

		m_Devices.push_back(device);

		emit signalVirtualDeviceCreated(device);
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
		if (!source.contains("devices"))
		{
			PS_LOG_ERROR(VirtualInputManager) << "Missing \"devices\" array in configuration.";

			return false;
		}

		QJsonArray devices_array = source["devices"].toArray();
		if (devices_array.isEmpty())
		{
			return true;
		}

		for (int i = 0; i < devices_array.size(); ++i)
		{
			QJsonObject device_object = devices_array.at(i).toObject();
			if (device_object.isEmpty())
			{
				PS_LOG_ERROR(VirtualInputManager) << "Not a device object.";

				return false;
			}

			VirtualInputDevicePtr device;

			if (i < m_Devices.size())
			{
				device = m_Devices[i];
			}
			else
			{
				device = createDevice();
			}

			if (device == nullptr ||
				!device->deserialize(m_RawInput, m_UsbController, device_object, version))
			{
				PS_LOG_ERROR(VirtualInputManager) << "Failed to load virtual device.";

				return false;
			}
		}

		return true;
	}

}; // namespace Pitstop