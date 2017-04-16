#include <Core/Main.h>

#include "DriverBase.h"
#include "DriverInfo.h"

extern Pitstop::DriverBase* CreateDriver();

static Pitstop::DriverBase* g_Driver = nullptr;

extern "C" int32_t Pitstop_Initialize()
{
	g_Driver = CreateDriver();
	if (g_Driver == nullptr)
	{
		return 0;
	}

	return 1;
}

extern "C" void Pitstop_Destroy()
{
	if (g_Driver == nullptr)
	{
		return;
	}

	delete g_Driver;
	g_Driver = nullptr;
}

extern "C" Pitstop::DriverInfo* Pitstop_GetInfo()
{
	if (g_Driver == nullptr)
	{
		return nullptr;
	}

	return g_Driver->getInfo();
}