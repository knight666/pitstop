#pragma once

#include <Core/Main.h>
#include "Input/Container/ContainerDevice.h"

namespace Pitstop {

	class ContainerManager
		: public QObject
	{

		Q_OBJECT

	public:

		ContainerManager();
		~ContainerManager();

		QSharedPointer<ContainerDevice> findContainerByIdentifier(const QString& identifier) const;
		QSharedPointer<ContainerDevice> findContainerByGuidAndInstanceIdentifier(const GUID& guid, const QString& instancePath) const;
		QSharedPointer<ContainerDevice> findContainerByDevicePath(const QString& devicePath) const;

		bool initialize();
		bool updateContainers();

	private:

		QHash<QString, QSharedPointer<ContainerDevice>> m_Containers;

	}; // class ContainerManager

}; // namespace Pitstop