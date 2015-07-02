#include "Input/RawInputManager.h"

#include "Input/Process/InputProcessorBase.h"
#include "Input/RawInputJoystick.h"

namespace Pitstop {

	RawInputManager::RawInputManager()
		: m_Initialized(false)
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
			if (::GetRawInputDeviceInfoW(
				device_info.hDevice,
				RIDI_DEVICEINFO,
				&info,
				(PUINT)&info.cbSize) == (UINT)-1)
			{
				continue;
			}

			// Get device name

			UINT device_name_size = 0;
			if (::GetRawInputDeviceInfoW(
				device_info.hDevice,
				RIDI_DEVICENAME,
				nullptr,
				&device_name_size) != 0)
			{
				continue;
			}

			QVector<ushort> device_name_data;
			device_name_data.resize((int)device_name_size);
			::GetRawInputDeviceInfoW(
				device_info.hDevice,
				RIDI_DEVICENAME,
				&device_name_data[0],
				&device_name_size);

			QString device_name = QString::fromUtf16(&device_name_data[0], device_name_data.size() * sizeof(ushort));

			// Only interested in joysticks

			if (device_info.dwType != RIM_TYPEHID ||
				device_name.indexOf("VID_") == -1 ||
				device_name.indexOf("PID_") == -1)
			{
				continue;
			}

			RawInputJoystick* joystick = new RawInputJoystick(*this, device_info.hDevice, info, window, device_name);
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

		m_Initialized = true;

		return true;
	}

	void RawInputManager::processInput(LPARAM lParam, WPARAM wParam)
	{
		// Get raw input message

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

		// Check if input was for a Human Interface device

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

		// Update joystick

		RawInputJoystick* joystick = getJoystickByHandle(raw_input->header.hDevice);
		if (joystick != nullptr)
		{
			joystick->process(*raw_input);
		}

		emit signalJoystickInput(joystick);
	}

	void RawInputManager::processConnectionChanged(LPARAM lParam, WPARAM wParam)
	{
		// Check if initialized

		if (!m_Initialized)
		{
			return;
		}

		HANDLE device = (HANDLE)lParam;

		// Match joystick by GUID

		if (wParam == GIDC_ARRIVAL)
		{
			// Check if the device is a Human Interface Device

			RID_DEVICE_INFO info = { 0 };
			info.cbSize = sizeof(RID_DEVICE_INFO);
			if (::GetRawInputDeviceInfoW(
				device,
				RIDI_DEVICEINFO,
				&info,
				(PUINT)&info.cbSize) == (UINT)-1 ||
				info.dwType != RIM_TYPEHID)
			{
				return;
			}

			// Get device path

			UINT device_path_size = 0;
			if (::GetRawInputDeviceInfoW(
				device,
				RIDI_DEVICENAME,
				nullptr,
				&device_path_size) != 0)
			{
				return;
			}

			QVector<ushort> device_path_data;
			device_path_data.resize((int)device_path_size);
			::GetRawInputDeviceInfoW(
				device,
				RIDI_DEVICENAME,
				&device_path_data[0],
				&device_path_size);

			QString device_path = QString::fromUtf16(
				&device_path_data[0],
				device_path_data.size() * sizeof(ushort));

			// Only interested in joysticks

			if (device_path.indexOf("VID_") == -1 ||
				device_path.indexOf("PID_") == -1 )
			{
				return;
			}

			// Extract GUID

			QRegExp extract_guid("(\\{.+\\})");
			if (extract_guid.indexIn(device_path) < 0)
			{
				return;
			}

			GUID guid;
			::CLSIDFromString(extract_guid.cap(1).utf16(), &guid);

			// Update joystick

			RawInputJoystick* joystick = nullptr;

			for (QHash<HANDLE, RawInputJoystick*>::iterator it = m_Joysticks.begin(); it != m_Joysticks.end(); ++it)
			{
				if (it.value()->getGuid() == guid &&
					it.value()->getType() != RawInputJoystick::Type::XInput)
				{
					joystick = it.value();

					m_Joysticks.erase(it);
					m_Joysticks.insert(device, joystick);

					break;
				}
			}

			if (joystick != nullptr)
			{
				joystick->setConnected(device, true);
			}
		}
		else if (wParam == GIDC_REMOVAL)
		{
			// Update joystick

			RawInputJoystick* joystick = nullptr;

			for (QHash<HANDLE, RawInputJoystick*>::iterator it = m_Joysticks.begin(); it != m_Joysticks.end(); ++it)
			{
				if (it.value()->getHandle() == device)
				{
					joystick = it.value();

					break;
				}
			}

			if (joystick != nullptr)
			{
				joystick->setConnected(device, false);
			}
		}
	}

	RawInputJoystick* RawInputManager::getJoystick() const
	{
		for (RawInputJoystick* joystick : m_Joysticks)
		{
			if (joystick->getType() != RawInputJoystick::Type::XInput)
			{
				return joystick;
			}
		}

		return nullptr;
	}

	RawInputJoystick* RawInputManager::getJoystickByHandle(HANDLE device) const
	{
		QHash<HANDLE, RawInputJoystick*>::const_iterator found = m_Joysticks.find(device);
		if (found != m_Joysticks.end())
		{
			return found.value();
		}

		return nullptr;
	}

	InputProcessorBase* RawInputManager::createInputProcessor(RawInputJoystick& joystick)
	{
		uint32_t key = (joystick.getVendorIdentifier() << 16) | joystick.getProductIdentifier();
		QHash<uint32_t, std::function<InputProcessorBase::FactoryMethod>>::iterator found = m_InputProcessorFactories.find(key);
		if (found != m_InputProcessorFactories.end())
		{
			return found.value()(joystick);
		}

		return nullptr;
	}

	void RawInputManager::registerInputProcessor(uint16_t vendor, uint16_t product, InputProcessorBase::FactoryMethod method)
	{
		uint32_t key = (vendor << 16) | product;

		m_InputProcessorFactories.insert(key, std::bind(method, std::placeholders::_1));
	}

	void RawInputManager::processInputMessage(const RAWINPUT& message, HANDLE device)
	{
		QHash<HANDLE, RawInputJoystick*>::iterator found = m_Joysticks.find(device);
		if (found != m_Joysticks.end())
		{
			RawInputJoystick* joystick = found.value();

			joystick->process(message);
		}
	}

}; // namespace Pitstop