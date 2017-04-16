#pragma once

#include <Core/Main.h>

namespace Pitstop {

	struct DriverInfo
	{
		uint16_t version;
		char name[256];
		uint16_t vendor;
		uint16_t product;
	};

	struct DigitalInput
	{
		USAGE identifier;
		char name[32];
		bool value;
	};

	struct AnalogInput
	{
		USAGE identifier;
		char name[32];
		float minimum;
		float maximum;
		float value;
	};

};