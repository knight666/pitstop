#pragma once

#include "Base/Main.h"

namespace Pitstop {

	enum class Levels
	{
		Information,
		Trace,
		Warning,
		Error,
		Fatal
	};

	inline const char* LevelToString(Levels level)
	{
		static const char* LevelToStringMap[] = {
			"INFO",
			"TRACE",
			"WARN",
			"ERROR",
			"FATAL"
		};
		return LevelToStringMap[(size_t)level];
	}

}; // namespace Pitstop