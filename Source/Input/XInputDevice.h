#pragma once

#include "Base/Main.h"

namespace Pitstop {

	class XInputDevice
	{

	public:

		XInputDevice();
		~XInputDevice();

		bool setup(const GUID& guid, size_t controllerIndex);

	private:

		QString m_DevicePath;
		HDEVINFO m_DeviceInfo;
		HANDLE m_DeviceHandle;

	}; // class XInputDevice

}; // namespace Pitstop