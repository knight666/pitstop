#include "Input/RawInputManager.h"

#include "Input/RawInputJoystick.h"
#include "Input/XInputDevice.h"

namespace Pitstop {

	RawInputManager::RawInputManager()
	{
	}

	RawInputManager::~RawInputManager()
	{
		qDeleteAll(m_Joysticks);
	}

	bool RawInputManager::initialize(HWND window)
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

			RawInputJoystick* joystick = new RawInputJoystick(device_info.hDevice, info, window, device_name);
			if (joystick->setup())
			{
				m_Joysticks.insert(joystick->getHandle(), joystick);
			}
			else
			{
				delete joystick;
			}
		}

		if (m_Joysticks.size() > 0)
		{
			size_t controller = 0;

			// Register raw input devices

			QVector<RAWINPUTDEVICE> device_list;
			for (RawInputJoystick* joystick : m_Joysticks)
			{
				if (joystick->getType() != RawInputJoystick::Type::XInput)
				{
					device_list.push_back(joystick->getDevice());
				}

				XInputDevice* xinput = new XInputDevice(0);
				bool success = xinput->attach(*joystick);
				xinput->setPluggedIn(true);
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
		UINT raw_size = 0;
		if (::GetRawInputData(
			(HRAWINPUT)lParam,
			RID_INPUT,
			NULL,
			&raw_size,
			sizeof(RAWINPUTHEADER)) == (UINT)-1)
		{
			return;
		}

		QVector<uint8_t> raw_data;
		raw_data.resize(raw_size);
		RAWINPUT* raw_input = (RAWINPUT*)&raw_data[0];

		if (::GetRawInputData(
			(HRAWINPUT)lParam,
			RID_INPUT,
			raw_input,
			&raw_size,
			sizeof(RAWINPUTHEADER)) == (UINT)-1 ||
			raw_input->header.dwType != RIM_TYPEHID)
		{
			return;
		}

		QHash<HANDLE, RawInputJoystick*>::iterator found = m_Joysticks.find(raw_input->header.hDevice);
		if (found != m_Joysticks.end())
		{
			RawInputJoystick* joystick = found.value();

			joystick->process(*raw_input);
		}
	}

}; // namespace Pitstop