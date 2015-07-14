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