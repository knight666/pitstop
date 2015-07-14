#pragma once

#include "Base/Main.h"
#include "Input/Container/ContainerDevice.h"

namespace Pitstop {

	class ContainerManager
		: public QObject
	{

		Q_OBJECT

	public:

		ContainerManager();
		~ContainerManager();

		QSharedPointer<ContainerDevice> findContainer(const QString& identifier) const;

		bool initialize();
		bool updateContainers();

	private:

		QHash<QString, QSharedPointer<ContainerDevice>> m_Containers;

	}; // class ContainerManager

}; // namespace Pitstop