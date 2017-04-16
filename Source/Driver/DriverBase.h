#pragma once

#include <Core/Main.h>

#include "DriverInfo.h"

namespace Pitstop {

	class DriverBase
	{

	public:

		DriverBase();
		virtual ~DriverBase();

		DriverInfo* GetInfo() { return &m_Info; }

	private:

		DriverInfo m_Info;

	};

};