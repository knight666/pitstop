#include "Input/Container/ContainerDevice.h"

namespace Pitstop {

	ContainerDevice::ContainerDevice(
			ContainerManager& manager,
			const QString& identifier,
			HDEVINFO info,
			const SP_DEVINFO_DATA& infoData)
		: m_Manager(manager)
		, m_Identifier(identifier)
		, m_Info(info)
		, m_InfoData(infoData)
	{
	}

	ContainerDevice::~ContainerDevice()
	{
	}

}; // namespace Pitstop