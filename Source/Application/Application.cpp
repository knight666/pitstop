#include "Application/Application.h"

#include <QtGui/QWindow>
#include <QtGui/QGuiApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

#include "Application/MainWindow.h"
#include "Input/Process/InputProcessorDualShock4.h"
#include "Input/Process/InputProcessorFFBWheel.h"
#include "Input/Usb/UsbController.h"
#include "Input/RawInputManager.h"
#include "Input/VirtualInputManager.h"
#include "Logging/SinkFile.h"

namespace Pitstop {

	Application* Application::s_Instance = nullptr;

	Application& Application::get()
	{
		return *s_Instance;
	}

	Application::Application(int& argc, char** argv, int flags /*= ApplicationFlags*/)
		: QApplication(argc, argv, flags)
		, m_RawInput(new RawInputManager())
		, m_UsbController(new UsbController())
		, m_VirtualInput(new VirtualInputManager(*m_RawInput))
		, m_MainWindow(new MainWindow(*m_RawInput, *m_UsbController, *m_VirtualInput))
	{
		s_Instance = this;

		setApplicationName("Pitstop");

		Logger::initialize();

		PS_LOG_INFO(Application) << "Initializing application.";

		installNativeEventFilter(this);

		m_RawInput->registerInputProcessor<InputProcessorDualShock4>();
		m_RawInput->registerInputProcessor<InputProcessorFFBWheel>();
	}

	Application::~Application()
	{
		PS_LOG_INFO(Application) << "Closing application.";

		delete m_VirtualInput;
		delete m_UsbController;
		delete m_RawInput;
		delete m_MainWindow;

		Logger::destroy();

		s_Instance = nullptr;
	}

	bool Application::saveConfiguration()
	{
		PS_LOG_INFO(Application) << "Saving configuration to disk. (version " << (size_t)SERIALIZATION_VERSION << ")";

		QJsonObject configuration_root;

		configuration_root["version"] = SERIALIZATION_VERSION;

		QJsonObject virtual_input_object;
		if (!m_VirtualInput->serialize(virtual_input_object, SERIALIZATION_VERSION))
		{
			PS_LOG_ERROR(Application) << "Failed to save virtual input devices to configuration.";

			return false;
		}
		configuration_root["virtualInput"] = virtual_input_object;

		QJsonDocument configuration_document(configuration_root);

		QDir app_directory(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));

		QFile configuration_file(app_directory.absoluteFilePath("configuration.json"));
		if (!configuration_file.open(QIODevice::WriteOnly))
		{
			PS_LOG_ERROR(Application) << "Failed to open \"configuration.json\" for writing.";

			return false;
		}

		configuration_file.write(configuration_document.toJson());
		configuration_file.close();

		PS_LOG_INFO(Application) << "Save successful.";

		return true;
	}

	bool Application::loadConfiguration()
	{
		PS_LOG_INFO(Application) << "Loading configuration from disk.";

		QDir app_directory(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));

		QFile configuration_file(app_directory.absoluteFilePath("configuration.json"));
		if (!configuration_file.open(QIODevice::ReadOnly))
		{
			PS_LOG_ERROR(Application) << "Failed to open \"configuration.json\" for reading.";

			return false;
		}

		QJsonDocument configuration_document = QJsonDocument::fromJson(configuration_file.readAll());
		if (configuration_document.isNull())
		{
			PS_LOG_ERROR(Application) << "Failed to parse configuration document.";

			return false;
		}

		QJsonObject configuration_root = configuration_document.object();

		size_t version = (size_t)configuration_root["version"].toInt(0);

		PS_LOG_INFO(Application) << "Configuration version: " << (size_t)SERIALIZATION_VERSION << ".";

		QJsonObject virtual_input_object = configuration_root["virtualInput"].toObject();
		if (!virtual_input_object.isEmpty())
		{
			if (!m_VirtualInput->deserialize(virtual_input_object, version))
			{
				PS_LOG_ERROR(Application) << "Failed to load virtual input devices from configuration.";

				return false;
			}
		}

		configuration_file.close();

		PS_LOG_INFO(Application) << "Load successful.";

		return true;
	}

	int Application::run()
	{
		if (!m_RawInput->initialize((HWND)m_MainWindow->winId()))
		{
			PS_LOG_ERROR(RawInput) << "Failed to initialize raw input.";

			return false;
		}

		if (!m_UsbController->initialize())
		{
			PS_LOG_ERROR(UsbController) << "Failed to initialize virtual USB hub.";

			return false;
		}

		if (!loadConfiguration())
		{
			saveConfiguration();
		}

		m_MainWindow->show();

		return exec();
	}

	bool Application::nativeEventFilter(const QByteArray& eventType, void* message, long* result)
	{
		if (eventType != "windows_generic_MSG")
		{
			return false;
		}

		MSG* msg = (MSG*)message;

		switch (msg->message)
		{

		case WM_INPUT:
			m_RawInput->processInput(msg->lParam, msg->wParam);
			return true;

		case WM_INPUT_DEVICE_CHANGE:
			m_RawInput->processConnectionChanged(msg->lParam, msg->wParam);
			return true;

		default:
			return false;

		}
	}

}; // namespace Pitstop