#include "Input/RawInputManager.h"

#include <QtGui/QImage>

#include "Input/Process/InputProcessorBase.h"
#include "Input/RawInputJoystick.h"

namespace Pitstop {

	RawInputManager::RawInputManager()
		: m_Initialized(false)
		, m_Window(NULL)
	{
		addThumbnailImage(0x045E, 0x028E, "/media/images/xbox-360-controller.png");
		addThumbnailImage(0x045E, 0x02D1, "/media/images/xbox-one-controller.png");
	}

	RawInputManager::~RawInputManager()
	{
		PS_LOG_INFO(RawInput) << "Destroying raw input.";

		m_JoysticksByHandle.clear();
		m_JoysticksByPath.clear();
		m_JoystickThumbnails.clear();
	}

	bool RawInputManager::initialize(HWND window)
	{
		PS_LOG_INFO(RawInput) << "Initializing raw input.";

		m_Window = window;

		QVector<RAWINPUTDEVICELIST> device_info_list;

		UINT device_count = 0;
		if (::GetRawInputDeviceList(
			nullptr,
			&device_count,
			sizeof(RAWINPUTDEVICELIST)) == (UINT)-1)
		{
			DWORD errorCode = GetLastError();
			PS_LOG_ERROR(RawInput) << "Failed to retrieve raw input device list. (error: \"" << windowsErrorToString(errorCode) << "\" code: " << errorCode << ")";

			return false;
		}

		device_info_list.resize((int)device_count);
		::GetRawInputDeviceList(
			&device_info_list[0],
			&device_count,
			sizeof(RAWINPUTDEVICELIST));

		PS_LOG_INFO(RawInput) << "Check " << device_count << " device(s).";

		// Create devices

		QVector<RAWINPUTDEVICE> device_list;

		for (RAWINPUTDEVICELIST& device_info : device_info_list)
		{
			RawInputJoystickPtr joystick = createJoystick(device_info.hDevice);

			if (joystick != nullptr)
			{
				device_list.push_back(joystick->getDevice());

				emit signalJoystickConnected(joystick, true);
			}
		}

		PS_LOG_INFO(RawInput) << "Found " << device_list.size() << " joystick(s).";

		// Register devices

		if (device_list.size() > 0)
		{
			if (::RegisterRawInputDevices(
				&device_list[0],
				(UINT)device_list.size(),
				sizeof(RAWINPUTDEVICE)) == FALSE)
			{
				DWORD errorCode = GetLastError();
				PS_LOG_ERROR(RawInput) << "Failed to register raw input devices. (error: \"" << windowsErrorToString(errorCode) << "\" code: " << errorCode << ")";

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
		bool connected = wParam == GIDC_ARRIVAL;

		// Find and update joystick

		RawInputJoystickPtr joystick = createJoystick(device);
		if (joystick != nullptr)
		{
			joystick->setConnected(device, connected);

			emit signalJoystickConnected(joystick, connected);
		}
		else
		{
			PS_LOG_INFO(RawInput) << "Device " << device << " was " << (connected ? "connected" : "disconnected") << ".";
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

	RawInputJoystickPtr RawInputManager::getJoystickByPath(const QString& devicePath) const
	{
		QString unique_path = devicePath;

		QRegExp match_path(
			"\\\\\\\\?\\HID#"
			"VID_[0-9A-Fa-f]+"
			"&PID_[0-9A-Fa-f]+"
			"(&[A-Za-z]+_?[0-9A-Fa-f]+)");
		if (match_path.indexIn(unique_path) >= 0)
		{
			unique_path.replace(match_path.cap(1), "");
		}

		QHash<QString, RawInputJoystickPtr>::const_iterator found = m_JoysticksByPath.find(unique_path);
		if (found != m_JoysticksByPath.end())
		{
			return found.value();
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

	QVector<RawInputJoystickPtr> RawInputManager::getJoysticks() const
	{
		QVector<RawInputJoystickPtr> joysticks;

		for (const RawInputJoystickPtr& joystick : m_JoysticksByPath)
		{
			joysticks.push_back(joystick);
		}

		return joysticks;
	}

	QSharedPointer<QImage> RawInputManager::getJoystickThumbnail(uint16_t vendor, uint16_t product) const
	{
		uint32_t key = (vendor << 16) | product;

		QHash<uint32_t, QSharedPointer<QImage>>::const_iterator found = m_JoystickThumbnails.find(key);
		if (found != m_JoystickThumbnails.end())
		{
			return found.value();
		}

		return QSharedPointer<QImage>();
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

	QString RawInputManager::getDevicePath(HANDLE device)
	{
		UINT device_path_size = 0;
		if (::GetRawInputDeviceInfoW(
			device,
			RIDI_DEVICENAME,
			nullptr,
			&device_path_size) != 0)
		{
			return QString();
		}

		QVector<ushort> device_path_data;
		device_path_data.resize((int)device_path_size);
		::GetRawInputDeviceInfoW(
			device,
			RIDI_DEVICENAME,
			&device_path_data[0],
			&device_path_size);

		return QString::fromUtf16(
			&device_path_data[0],
			wcslen(&device_path_data[0]));
	}

	RawInputJoystickPtr RawInputManager::createJoystick(const QString& devicePath)
	{
		bool created = false;

		RawInputJoystickPtr joystick = getJoystickByPath(devicePath);

		if (joystick == nullptr)
		{
			joystick = RawInputJoystickPtr(
				new RawInputJoystick(
					*this,
					m_Window));

			created = true;
		}

		if (!joystick->setup(devicePath))
		{
			PS_LOG_ERROR(RawInputManager) << "Failed to setup joystick. (path: \"" << devicePath << "\")";
			joystick.clear();

			return joystick;
		}

		PS_LOG_INFO(RawInput) << "Setup joystick:";
		PS_LOG_INFO(RawInput) << "- Path: \"" << joystick->getDevicePath() << "\"";
		PS_LOG_INFO(RawInput) << "- Description: \"" << joystick->getDescription() << "\"";
		PS_LOG_INFO(RawInput) << "- Type: \"" << joystick->getType() << "\"";
		PS_LOG_INFO(RawInput) << "- Handle: " << joystick->getHandle() << "";

		if (created)
		{
			m_JoysticksByPath.insert(joystick->getUniquePath(), joystick);

			emit signalJoystickCreated(joystick);
		}

		return joystick;
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
			PS_LOG_ERROR(RawInput) << "Failed to retrieve properties from device " << device << ".";

			return RawInputJoystickPtr();
		}

		// Get device path

		QString device_path = getDevicePath(device);
		if (device_path.isEmpty())
		{
			PS_LOG_ERROR(RawInput) << "Failed to retrieve path from device " << device << ".";

			return RawInputJoystickPtr();
		}

		RawInputJoystickPtr joystick = createJoystick(device_path);

		if (joystick != nullptr &&
			joystick->initialize(device, info))
		{
			m_JoysticksByHandle.insert(device, joystick);
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

	void RawInputManager::addThumbnailImage(uint16_t vendor, uint16_t product, const QString& thumbnailPath)
	{
		uint32_t key = (vendor << 16) | product;

		QSharedPointer<QImage> thumbnail(new QImage);
		if (thumbnail->load(QDir::currentPath() + thumbnailPath))
		{
			m_JoystickThumbnails.insert(key, thumbnail);
		}
		else
		{
			thumbnail.reset();
		}
	}

}; // namespace Pitstop