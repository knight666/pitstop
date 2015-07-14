#pragma once

#include "Base/Main.h"

namespace Pitstop {

	class ContainerDevice
		: public QObject
	{

		Q_OBJECT

	public:

		ContainerDevice();
		~ContainerDevice();

	}; // class ContainerDevice

}; // namespace Pitstop