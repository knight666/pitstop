#include "Input/RawInputManager.h"

#include "Input/Process/InputProcessorBase.h"
#include "Input/RawInputJoystick.h"

namespace Pitstop {

	RawInputManager::RawInputManager()
		: m_Initialized(false)
		, m_Window(NULL)
	{
	}

	RawInputManager::~RawInputManager()
	{
		m_JoysticksByHandle.clear();
		m_JoysticksByGuid.clear();
	}

	bool RawInputManager::initialize(HWND window)
	{
		m_Window = window;

		QVector<RAWINPUTDEVICELIST> device_info_list;

		UINT device_count = 0;
		if (::GetRawInputDeviceList(
			nullptr,
			&device_count,
			sizeof(RAWINPUTDEVICELIST)) == (UINT)-1)
		{
			DWORD errorCode = GetLastError();

			return false;
		}

		device_info_list.resize((int)device_count);
		::GetRawInputDeviceList(
			&device_info_list[0],
			&device_count,
			sizeof(RAWINPUTDEVICELIST));

		// Create devices

		QVector<RAWINPUTDEVICE> device_list;

		for (RAWINPUTDEVICELIST& device_info : device_info_list)
		{
			RawInputJoystickPtr joystick = createJoystick(device_info.hDevice);

			if (joystick != nullptr)
			{
				if (joystick->getType() != RawInputJoystick::Type::XInput)
				{
					device_list.push_back(joystick->getDevice());
				}

				emit signalJoystickConnected(joystick, true);
			}
		}

		// Register devices

		if (device_list.size() > 0)
		{
			if (::RegisterRawInputDevices(
				&device_list[0],
				(UINT)device_list.size(),
				sizeof(RAWINPUTDEVICE)) == FALSE)
			{
				DWORD errorCode = GetLastError();

				return false;
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

		RawInputJoystickPtr joystick = getJoystickByHandle(raw_input->header.hDevice);
		if (joystick != nullptr)
		{
			joystick->process(*raw_input);
		}
	}

	void RawInputManager::processConnectionChanged(LPARAM lParam, WPARAM wParam)
	{
		// Check if initialized

		if (!m_Initialized)
		{
			return;
		}

		HANDLE device = (HANDLE)lParam;

		// Find and update joystick

		RawInputJoystickPtr joystick = createJoystick(device);
		if (joystick != nullptr)
		{
			bool connected = wParam == GIDC_ARRIVAL;

			joystick->setConnected(device, connected);

			emit signalJoystickConnected(joystick, connected);
		}
	}

	RawInputJoystickPtr RawInputManager::getJoystick() const
	{
		for (const RawInputJoystickPtr& joystick : m_JoysticksByHandle)
		{
			if (joystick->getType() != RawInputJoystick::Type::XInput)
			{
				return joystick;
			}
		}

		return RawInputJoystickPtr();
	}

	RawInputJoystickPtr RawInputManager::getJoystickByHandle(HANDLE device) const
	{
		QHash<HANDLE, RawInputJoystickPtr>::const_iterator found = m_JoysticksByHandle.find(device);
		if (found != m_JoysticksByHandle.end())
		{
			return found.value();
		}

		return RawInputJoystickPtr();
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

	RawInputJoystickPtr RawInputManager::createJoystick(HANDLE device)
	{
		// Check if handle is already known

		QHash<HANDLE, RawInputJoystickPtr>::iterator found_handle = m_JoysticksByHandle.find(device);
		if (found_handle != m_JoysticksByHandle.end())
		{
			return found_handle.value();
		}

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
			return RawInputJoystickPtr();
		}

		// Get device path

		UINT device_path_size = 0;
		if (::GetRawInputDeviceInfoW(
			device,
			RIDI_DEVICENAME,
			nullptr,
			&device_path_size) != 0)
		{
			return RawInputJoystickPtr();
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

		// Extract GUID

		QRegExp extract_guid("(\\{.+\\})");
		if (extract_guid.indexIn(device_path) < 0)
		{
			return RawInputJoystickPtr();
		}

		QString guid_string = extract_guid.cap(1);

		// Create or overwrite joystick

		RawInputJoystickPtr joystick;

		QHash<QString, RawInputJoystickPtr>::iterator found_guid = m_JoysticksByGuid.find(guid_string);
		if (found_guid != m_JoysticksByGuid.end())
		{
			joystick = found_guid.value();

			if (joystick->getType() != RawInputJoystick::Type::XInput)
			{
				// Same GUID, but not a virtual controller for a joystick

				return joystick;
			}
		}

		if (joystick == nullptr)
		{
			joystick.reset(
				new RawInputJoystick(
					*this,
					m_Window));
		}

		if (joystick->setup(device, info, device_path))
		{
			m_JoysticksByHandle.insert(joystick->getHandle(), joystick);
			m_JoysticksByGuid[joystick->getGuidString()] = joystick;
		}
		else
		{
			joystick.clear();
		}

		return joystick;
	}

	void RawInputManager::registerInputProcessor(uint16_t vendor, uint16_t product, InputProcessorBase::FactoryMethod method)
	{
		uint32_t key = (vendor << 16) | product;

		m_InputProcessorFactories.insert(key, std::bind(method, std::placeholders::_1));
	}

	void RawInputManager::processInputMessage(const RAWINPUT& message, HANDLE device)
	{
		QHash<HANDLE, RawInputJoystickPtr>::iterator found = m_JoysticksByHandle.find(device);
		if (found != m_JoysticksByHandle.end())
		{
			RawInputJoystickPtr& joystick = found.value();

			joystick->process(message);
		}
	}

}; // namespace Pitstop