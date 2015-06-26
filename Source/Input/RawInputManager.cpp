#include "Input/RawInputManager.h"

namespace Pitstop {

	RawInputManager::RawInputManager()
	{

	}

	RawInputManager::~RawInputManager()
	{

	}

	bool RawInputManager::initialize()
	{
		QVector<RAWINPUTDEVICELIST> device_info_list;
		QVector<RAWINPUTDEVICE> device_list;

		UINT device_count = 0;
		if (::GetRawInputDeviceList(nullptr, &device_count, sizeof(RAWINPUTDEVICELIST)) == (UINT)-1)
		{
			DWORD errorCode = GetLastError();

			return false;
		}

		device_info_list.resize((int)device_count);
		::GetRawInputDeviceList(&device_info_list[0], &device_count, sizeof(RAWINPUTDEVICELIST));

		device_list.resize((int)device_count);

		for (UINT i = 0; i < device_count; ++i)
		{
			RAWINPUTDEVICELIST& device_info = device_info_list[i];

			// Get device info

			RID_DEVICE_INFO info = { 0 };
			info.cbSize = sizeof(RID_DEVICE_INFO);
			if (::GetRawInputDeviceInfoW(device_info.hDevice, RIDI_DEVICEINFO, &info, (PUINT)&info.cbSize) == (UINT)-1)
			{
				continue;
			}

			// Get device name

			UINT device_name_size = 0;
			if (::GetRawInputDeviceInfoW(device_info.hDevice, RIDI_DEVICENAME, nullptr, &device_name_size) != 0)
			{
				continue;
			}

			QVector<ushort> device_name_data;
			device_name_data.resize((int)device_name_size);
			::GetRawInputDeviceInfoW(device_info.hDevice, RIDI_DEVICENAME, &device_name_data[0], &device_name_size);

			QString device_name = QString::fromUtf16(&device_name_data[0], device_name_data.size() * sizeof(ushort));

			// Only interested in joysticks

			if (device_info.dwType != RIM_TYPEHID ||
				device_name.indexOf("VID_") == -1 ||
				device_name.indexOf("PID_") == -1)
			{
				continue;
			}

			RAWINPUTDEVICE device = { 0 };
			device.hwndTarget = NULL;
		}

		return true;
	}

}; // namespace Pitstop