#include "Input/Container/ContainerManager.h"

namespace Pitstop {

	ContainerManager::ContainerManager()
	{
	}

	ContainerManager::~ContainerManager()
	{
		m_Containers.clear();
	}

	QSharedPointer<ContainerDevice> ContainerManager::findContainer(const QString& identifier) const
	{
		QHash<QString, QSharedPointer<ContainerDevice>>::const_iterator found = m_Containers.find(identifier);
		if (found != m_Containers.end())
		{
			return found.value();
		}

		return QSharedPointer<ContainerDevice>();
	}

	QSharedPointer<ContainerDevice> ContainerManager::findContainerByDevicePath(const QString& devicePath) const
	{
		QRegExp match_guid("#(\\{[A-Fa-f0-9]{8}-[A-Fa-f0-9]{4}-[A-Fa-f0-9]{4}-[A-Fa-f0-9]{4}-[A-Fa-f0-9]{12}\\})");
		int match_guid_location = match_guid.indexIn(devicePath);

		if (match_guid_location < 0)
		{
			return QSharedPointer<ContainerDevice>();
		}

		GUID device_guid = stringToGuid(match_guid.cap(1));

		QString device_instance_identifier = devicePath.mid(4, match_guid_location - 4);
		device_instance_identifier.replace("#", "\\");

		// Retrieve device information

		HDEVINFO device_info = ::SetupDiGetClassDevsW(
			&device_guid,
			NULL,
			NULL,
			DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
		if (device_info == INVALID_HANDLE_VALUE)
		{
			return QSharedPointer<ContainerDevice>();
		}

		SP_DEVINFO_DATA device_info_data = { 0 };
		device_info_data.cbSize = sizeof(SP_DEVINFO_DATA);

		// Find instance of class

		bool instance_found = false;

		for (DWORD i = 0;
			::SetupDiEnumDeviceInfo(
				device_info,
				i,
				&device_info_data) == TRUE;
			++i)
		{
			DWORD device_instance_size = 0;

			::SetupDiGetDeviceInstanceIdW(
				device_info,
				&device_info_data,
				nullptr,
				0,
				&device_instance_size);

			if (device_instance_size == 0)
			{
				continue;
			}

			QVector<WCHAR> device_instance_data(device_instance_size);

			if (::SetupDiGetDeviceInstanceIdW(
				device_info,
				&device_info_data,
				&device_instance_data[0],
				device_instance_data.size(),
				&device_instance_size) == TRUE)
			{
				if (device_instance_identifier.compare(
						QString::fromUtf16(&device_instance_data[0]),
						Qt::CaseInsensitive) == 0)
				{
					instance_found = true;

					break;
				}
			}
		}

		if (!instance_found)
		{
			return QSharedPointer<ContainerDevice>();
		}

		// Find container by identifier

		QString device_container = deviceGetRegistryProperty<QString>(device_info, device_info_data, SPDRP_BASE_CONTAINERID);

		return findContainer(device_container);
	}

	bool ContainerManager::initialize()
	{
		PS_LOG_INFO(ContainerManager) << "Initializing containers.";

		if (!updateContainers())
		{
			return false;
		}

		return true;
	}

	bool ContainerManager::updateContainers()
	{
		PS_LOG_INFO(ContainerManager) << "Updating containers.";

		HDEVINFO info = ::SetupDiGetClassDevsW(
			NULL,
			L"USB",
			NULL,
			DIGCF_ALLCLASSES | DIGCF_PRESENT);

		if (info == INVALID_HANDLE_VALUE)
		{
			DWORD last_error = GetLastError();
			PS_LOG_ERROR(ContainerManager) << "Failed to initialize device info. (error \"" << windowsErrorToString(last_error) << "\" code " << QString::number(last_error, 16) << ")";

			return false;
		}

		SP_DEVINFO_DATA info_data = { 0 };
		info_data.cbSize = sizeof(SP_DEVINFO_DATA);

		for (DWORD i = 0;
			::SetupDiEnumDeviceInfo(
				info,
				i,
				&info_data) == TRUE;
			++i)
		{
			QString identifier = deviceGetRegistryProperty<QString>(info, info_data, SPDRP_BASE_CONTAINERID);

			QHash<QString, QSharedPointer<ContainerDevice>>::const_iterator found = m_Containers.find(identifier);
			if (found == m_Containers.end())
			{
				QSharedPointer<ContainerDevice> container(
					new ContainerDevice(
						*this,
						identifier,
						info,
						info_data));

				PS_LOG_INFO(ContainerManager) << "Add container \"" << deviceGetRegistryProperty<QString>(info, info_data, SPDRP_LOCATION_INFORMATION) << "\". (identifier " << identifier << ")";

				m_Containers.insert(identifier, container);
			}
		}

		return true;
	}

}; // namespace Pitstop