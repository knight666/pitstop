#include "Input/RawInput/RawInputManager.h"

#include <QtGui/QImage>

#include "Input/Container/ContainerManager.h"
#include "Input/Process/InputProcessorBase.h"

namespace Pitstop {

	RawInputManager::RawInputManager(QSharedPointer<ContainerManager> containers)
		: m_Containers(containers)
		, m_Initialized(false)
		, m_Window(NULL)
	{
		addThumbnailImage(0x045E, 0x028E, "/media/images/xbox-360-controller.png");
		addThumbnailImage(0x045E, 0x02D1, "/media/images/xbox-one-controller.png");
	}

	RawInputManager::~RawInputManager()
	{
		PS_LOG_INFO(RawInputManager) << "Destroying raw input.";

		m_JoysticksByHandle.clear();
		m_JoysticksByIdentifier.clear();
		m_JoystickThumbnails.clear();
	}

	bool RawInputManager::initialize(HWND window)
	{
		PS_LOG_INFO(RawInputManager) << "Initializing raw input.";

		m_Window = window;

		if (!updateRegisteredDevices())
		{
			return false;
		}

		m_Initialized = true;

		return true;
	}

	bool RawInputManager::updateRegisteredDevices()
	{
		if (m_Initialized)
		{
			m_Containers->updateContainers();
		}

		PS_LOG_INFO(RawInputManager) << "Updating registered devices.";

		QVector<RAWINPUTDEVICELIST> device_info_list;

		UINT device_count = 0;
		if (::GetRawInputDeviceList(
			nullptr,
			&device_count,
			sizeof(RAWINPUTDEVICELIST)) == (UINT)-1)
		{
			PS_LOG_ERROR(RawInputManager) << "Failed to retrieve raw input device list." << PS_LOG_WINDOWS_ERROR;

			return false;
		}

		device_info_list.resize((int)device_count);
		::GetRawInputDeviceList(
			&device_info_list[0],
			&device_count,
			sizeof(RAWINPUTDEVICELIST));

		PS_LOG_INFO(RawInputManager) << "Check " << device_count << " device(s).";

		// Create devices

		QVector<RAWINPUTDEVICE> device_list;

		for (RAWINPUTDEVICELIST& device_info : device_info_list)
		{
			QSharedPointer<RawInputJoystick> joystick = createJoystick(device_info.hDevice);

			if (joystick != nullptr)
			{
				device_list.push_back(joystick->getDevice());

				emit signalJoystickConnected(joystick, true);
			}
		}

		PS_LOG_INFO(RawInputManager) << "Found " << device_list.size() << " joystick(s).";

		// Register devices

		if (device_list.size() > 0)
		{
			if (::RegisterRawInputDevices(
				&device_list[0],
				(UINT)device_list.size(),
				sizeof(RAWINPUTDEVICE)) == FALSE)
			{
				PS_LOG_ERROR(RawInputManager) << "Failed to register raw input devices." << PS_LOG_WINDOWS_ERROR;

				return false;
			}
		}

		return true;
	}

	void RawInputManager::processInput(LPARAM lParam, WPARAM wParam)
	{
		// Retrieve raw input data

		UINT raw_size = 0;
		if (::GetRawInputData(
			(HRAWINPUT)lParam,
			RID_INPUT,
			NULL,
			&raw_size,
			sizeof(RAWINPUTHEADER)) == (UINT)-1)
		{
			PS_LOG_ERROR(RawInputManager) << "Failed to retrieve raw input data." << PS_LOG_WINDOWS_ERROR;

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
			sizeof(RAWINPUTHEADER)) == (UINT)-1)
		{
			PS_LOG_ERROR(RawInputManager) << "Failed to retrieve raw input data." << PS_LOG_WINDOWS_ERROR;

			return;
		}

		// Check if input was for a Human Interface device
		
		if (raw_input->header.dwType != RIM_TYPEHID)
		{
			return;
		}

		// Update joystick

		QSharedPointer<RawInputJoystick> joystick = getJoystickByHandle(raw_input->header.hDevice);
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

		QString device_string;
		device_string.sprintf("0x%08X", device);
		PS_LOG_TRACE(RawInputManager) << "processConnectionChanged device " << device_string << " connected " << (connected ? "yes" : "no") << ".";

		// Find and update joystick

		QSharedPointer<RawInputJoystick> joystick = createJoystick(device);
		if (joystick != nullptr)
		{
			joystick->setConnected(device, connected);

			emit signalJoystickConnected(joystick, connected);
		}
		else
		{
			PS_LOG_INFO(RawInputManager) << "Device " << device << " was " << (connected ? "connected" : "disconnected") << ".";
		}
	}

	QSharedPointer<RawInputJoystick> RawInputManager::getJoystick() const
	{
		for (const QSharedPointer<RawInputJoystick>& joystick : m_JoysticksByHandle)
		{
			if (joystick->getType() != RawInputJoystick::Type::XInput)
			{
				return joystick;
			}
		}

		return QSharedPointer<RawInputJoystick>();
	}

	QSharedPointer<RawInputJoystick> RawInputManager::getJoystickByIdentifier(const QString& identifier) const
	{
		QHash<QString, QSharedPointer<RawInputJoystick>>::const_iterator found = m_JoysticksByIdentifier.find(identifier);
		if (found != m_JoysticksByIdentifier.end())
		{
			return found.value();
		}

		return QSharedPointer<RawInputJoystick>();
	}

	QSharedPointer<RawInputJoystick> RawInputManager::getJoystickByDevicePath(const QString& devicePath) const
	{
		QSharedPointer<ContainerDevice> container = m_Containers->findContainerByDevicePath(devicePath);
		if (container == nullptr)
		{
			return QSharedPointer<RawInputJoystick>();
		}

		return getJoystickByIdentifier(container->getIdentifier());
	}

	QSharedPointer<RawInputJoystick> RawInputManager::getJoystickByHandle(HANDLE device) const
	{
		QHash<HANDLE, QSharedPointer<RawInputJoystick>>::const_iterator found = m_JoysticksByHandle.find(device);
		if (found != m_JoysticksByHandle.end())
		{
			return found.value();
		}

		return QSharedPointer<RawInputJoystick>();
	}

	QVector<QSharedPointer<RawInputJoystick>> RawInputManager::getJoysticks() const
	{
		QVector<QSharedPointer<RawInputJoystick>> joysticks;

		for (const QSharedPointer<RawInputJoystick>& joystick : m_JoysticksByIdentifier)
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

		return QString::fromUtf16(&device_path_data[0]);
	}

	QSharedPointer<RawInputJoystick> RawInputManager::createJoystick(const QString& devicePath)
	{
		bool created = false;

		QSharedPointer<RawInputJoystick> joystick = getJoystickByDevicePath(devicePath);

		if (joystick == nullptr)
		{
			// Ensure container is available

			m_Containers->updateContainers();

			// Create joystick

			joystick = QSharedPointer<RawInputJoystick>(
				new RawInputJoystick(
					*this,
					m_Window));

			created = true;
		}

		if (!joystick->setup(*m_Containers, devicePath))
		{
			PS_LOG_ERROR(RawInputManager) << "Failed to setup joystick. (path \"" << devicePath << "\")";
			joystick.clear();

			return joystick;
		}

		PS_LOG_INFO(RawInputManager) << "Setup joystick:";
		PS_LOG_INFO(RawInputManager) << "- Path: \"" << joystick->getDevicePath() << "\"";
		PS_LOG_INFO(RawInputManager) << "- Instance: \"" << joystick->getInstancePath() << "\"";
		PS_LOG_INFO(RawInputManager) << "- Description: \"" << joystick->getDescription() << "\"";
		PS_LOG_INFO(RawInputManager) << "- Type: \"" << joystick->getType() << "\"";
		PS_LOG_INFO(RawInputManager) << "- Handle: " << joystick->getHandle() << "";

		if (created)
		{
			m_JoysticksByIdentifier.insert(joystick->getIdentifier(), joystick);

			emit signalJoystickCreated(joystick);
			emit signalJoystickConnected(joystick, true);
		}

		return joystick;
	}

	QSharedPointer<RawInputJoystick> RawInputManager::createJoystick(HANDLE device)
	{
		// Check if handle is already known

		QHash<HANDLE, QSharedPointer<RawInputJoystick>>::iterator found_handle = m_JoysticksByHandle.find(device);
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
			PS_LOG_ERROR(RawInputManager) << "Failed to retrieve properties from device " << device << "." << PS_LOG_WINDOWS_ERROR;

			return QSharedPointer<RawInputJoystick>();
		}

		// Get device path

		QString device_path = getDevicePath(device);
		if (device_path.isEmpty())
		{
			PS_LOG_ERROR(RawInputManager) << "Failed to retrieve path from device " << device << "." << PS_LOG_WINDOWS_ERROR;

			return QSharedPointer<RawInputJoystick>();
		}

		// Create joystick

		QSharedPointer<RawInputJoystick> joystick = createJoystick(device_path);

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

	QSharedPointer<RawInputJoystick> RawInputManager::createJoystick(const QJsonObject& serialized)
	{
		// Joystick from path

		if (!serialized.contains("path") ||
			!serialized["path"].isString())
		{
			PS_LOG_ERROR(RawInputManager) << "Missing required field \"path\"";

			return QSharedPointer<RawInputJoystick>();
		}

		QSharedPointer<RawInputJoystick> joystick = createJoystick(serialized["path"].toString());

		if (joystick == nullptr)
		{
			PS_LOG_ERROR(RawInputManager) << "Failed to create joystick.";

			return QSharedPointer<RawInputJoystick>();
		}

		// Description

		if (serialized.contains("description") &&
			serialized["description"].isString())
		{
			joystick->setDescription(serialized["description"].toString());
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