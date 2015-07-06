#pragma once

#include "Base/Main.h"

namespace Pitstop {

	QString windowsErrorToString(DWORD error, HMODULE module = NULL);

}; // namespace Pitstop