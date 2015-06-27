#pragma once

#include "Base/Main.h"

namespace Pitstop {

	class XInputDevice
	{

	public:

		XInputDevice();
		~XInputDevice();

		bool setup(size_t controllerIndex);

	private:

		QString m_DevicePath;
		HANDLE m_DeviceHandle;

	}; // class XInputDevice

}; // namespace Pitstop