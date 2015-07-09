#pragma once

#include "Base/Main.h"

namespace Pitstop {

	QString windowsErrorToString(DWORD error, HMODULE module = NULL);

	GUID stringToGuid(const QString& guid);
	QString guidToString(const GUID& guid);

}; // namespace Pitstop