#include "Input/RawInputManager.h"

#include "Input/RawInputJoystick.h"

namespace Pitstop {

	RawInputManager::RawInputManager()
	{
	}

	RawInputManager::~RawInputManager()
	{
		qDeleteAll(m_Joysticks);
	}

	bool RawInputManager::initialize()
	{
		QVector<RAWINPUTDEVICELIST> device_info_list;

		UINT device_count = 0;
		if (::GetRawInputDeviceList(nullptr, &device_count, sizeof(RAWINPUTDEVICELIST)) == (UINT)-1)
		{
			DWORD errorCode = GetLastError();

			return false;
		}

		device_info_list.resize((int)device_count);
		::GetRawInputDeviceList(&device_info_list[0], &device_count, sizeof(RAWINPUTDEVICELIST));

		for (RAWINPUTDEVICELIST& device_info : device_info_list)
		{
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

			RawInputJoystick* joystick = new RawInputJoystick(device_info.hDevice, info, NULL, device_name);

			m_Joysticks.insert(joystick->getHandle(), joystick);
		}

		if (m_Joysticks.size() > 0)
		{
			// Register raw input devices

			QVector<RAWINPUTDEVICE> device_list;
			for (RawInputJoystick* joystick : m_Joysticks)
			{
				if (joystick->getType() != RawInputJoystick::Type::XInput)
				{
					device_list.push_back(joystick->getDevice());
				}
			}

			if (device_list.size() > 0)
			{
				if (::RegisterRawInputDevices(&device_list[0], (UINT)device_list.size(), sizeof(RAWINPUTDEVICE)) == FALSE)
				{
					DWORD errorCode = GetLastError();

					return false;
				}
			}
		}

		return true;
	}

	void RawInputManager::processInputMessage(WPARAM wParam, LPARAM lParam)
	{
		UINT size;
		if (::GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &size, sizeof(RAWINPUTHEADER)) == (UINT)-1)
		{
			return;
		}

		QByteArray raw_data;
		raw_data.resize(size);
		RAWINPUT* input = (RAWINPUT*)&raw_data[0];

		if (::GetRawInputData((HRAWINPUT)lParam, RID_INPUT, input, &size, sizeof(RAWINPUTHEADER)) == (UINT)-1 ||
			input->header.dwType != RIM_TYPEHID)
		{
			return;
		}

		QHash<HANDLE, RawInputJoystick*>::iterator found = m_Joysticks.find(input->header.hDevice);
		if (found != m_Joysticks.end())
		{
			RawInputJoystick* joystick = found.value();

			int i = 0;
		}
	}

}; // namespace Pitstop