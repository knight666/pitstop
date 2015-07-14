#pragma once

#include "Base/Main.h"

namespace Pitstop {

	class ContainerManager;

	class ContainerDevice
		: public QObject
	{

		Q_OBJECT

	public:

		ContainerDevice(
			ContainerManager& manager,
			const QString& identifier,
			HDEVINFO info,
			const SP_DEVINFO_DATA& infoData);
		~ContainerDevice();

		const QString& getIdentifier() const { return m_Identifier; }

		template <typename ValueType>
		ValueType getRegistryProperty(DWORD key);

	private:

		ContainerManager& m_Manager;
		QString m_Identifier;
		HDEVINFO m_Info;
		SP_DEVINFO_DATA m_InfoData;

	}; // class ContainerDevice

	template <typename ValueType>
	inline ValueType ContainerDevice::getRegistryProperty(DWORD key)
	{
		return deviceGetRegistryProperty<ValueType>(m_Info, m_InfoData, key);
	}

}; // namespace Pitstop