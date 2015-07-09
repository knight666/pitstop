#include "Serialization/ConfigurationManager.h"

#include "Serialization/ConfigurationBase.h"

namespace Pitstop {

	ConfigurationManager::ConfigurationManager()
		: m_Locked(false)
	{
	}

	ConfigurationManager::~ConfigurationManager()
	{
		disconnect(
			this, SLOT(slotSaveConfiguration()));

		disconnect(
			this, SLOT(slotLoadConfiguration()));

		m_Installed.clear();
	}

	void ConfigurationManager::addEventDispatcher(ConfigurationEventDispatcher& eventDispatcher)
	{
		for (QVector<ConfigurationEventDispatcher*>::iterator it = m_EventDispatchers.begin(); it != m_EventDispatchers.end(); ++it)
		{
			if (*it == &eventDispatcher)
			{
				PS_LOG_WARN(ConfigurationManager) << "Event dispatcher was already installed.";

				return;
			}
		}

		connect(
			&eventDispatcher, SIGNAL(signalSaveConfiguration()),
			this, SLOT(slotSaveConfiguration()));

		connect(
			&eventDispatcher, SIGNAL(signalLoadConfiguration()),
			this, SLOT(slotLoadConfiguration()));

		m_EventDispatchers.push_back(&eventDispatcher);
	}

	void ConfigurationManager::removeEventDispatcher(ConfigurationEventDispatcher& eventDispatcher)
	{
		for (QVector<ConfigurationEventDispatcher*>::iterator it = m_EventDispatchers.begin(); it != m_EventDispatchers.end(); ++it)
		{
			if (*it == &eventDispatcher)
			{
				disconnect(
					&eventDispatcher, SIGNAL(signalSaveConfiguration()),
					this, SLOT(slotSaveConfiguration()));

				disconnect(
					&eventDispatcher, SIGNAL(signalLoadConfiguration()),
					this, SLOT(slotLoadConfiguration()));

				m_EventDispatchers.erase(it);

				return;
			}
		}

		PS_LOG_WARN(ConfigurationManager) << "Configuration was already removed.";
	}

	void ConfigurationManager::install(ConfigurationBase& configuration)
	{
		for (QVector<ConfigurationBase*>::iterator it = m_Installed.begin(); it != m_Installed.end(); ++it)
		{
			if (*it == &configuration)
			{
				PS_LOG_WARN(ConfigurationManager) << "Configuration was already installed.";

				return;
			}
		}

		addEventDispatcher(configuration);

		m_Installed.push_back(&configuration);
	}

	void ConfigurationManager::uninstall(ConfigurationBase& configuration)
	{
		for (QVector<ConfigurationBase*>::iterator it = m_Installed.begin(); it != m_Installed.end(); ++it)
		{
			if (*it == &configuration)
			{
				removeEventDispatcher(configuration);

				m_Installed.erase(it);

				return;
			}
		}

		PS_LOG_WARN(ConfigurationManager) << "Configuration was already uninstalled.";
	}

	bool ConfigurationManager::save()
	{
		if (m_Locked)
		{
			return true;
		}

		ScopedLock locked(m_Locked);

		PS_LOG_INFO(ConfigurationManager) << "Saving configuration to disk. (version " << CONFIGURATION_VERSION << ")";

		QJsonObject configuration_root;

		configuration_root["version"] = (double)CONFIGURATION_VERSION;

		for (ConfigurationBase* installed : m_Installed)
		{
			if (!installed->saveConfiguration(configuration_root, CONFIGURATION_VERSION))
			{
				PS_LOG_INFO(ConfigurationManager) << "Failed to save configuration.";

				return false;
			}
		}

		QJsonDocument configuration_document(configuration_root);

		QDir app_directory(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));

		QFile configuration_file(app_directory.absoluteFilePath("configuration.json"));
		if (!configuration_file.open(QIODevice::WriteOnly))
		{
			PS_LOG_ERROR(ConfigurationManager) << "Failed to open \"configuration.json\" for writing.";

			return false;
		}

		configuration_file.write(configuration_document.toJson());
		configuration_file.close();

		PS_LOG_INFO(ConfigurationManager) << "Save successful.";

		return true;
	}

	bool ConfigurationManager::load()
	{
		if (m_Locked)
		{
			return true;
		}

		ScopedLock locked(m_Locked);

		PS_LOG_INFO(ConfigurationManager) << "Loading configuration from disk.";

		QDir app_directory(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));

		QFile configuration_file(app_directory.absoluteFilePath("configuration.json"));
		if (!configuration_file.open(QIODevice::ReadOnly))
		{
			PS_LOG_ERROR(ConfigurationManager) << "Failed to open \"configuration.json\" for reading.";

			return false;
		}

		QJsonParseError errors;
		QJsonDocument configuration_document = QJsonDocument::fromJson(configuration_file.readAll(), &errors);
		configuration_file.close();

		if (errors.error != QJsonParseError::NoError)
		{
			PS_LOG_ERROR(ConfigurationManager) << "Failed to parse configuration document. (error: \"" << errors.errorString() << "\")";

			return false;
		}

		QJsonObject configuration_root = configuration_document.object();

		size_t version = (size_t)configuration_root["version"].toInt(0);

		PS_LOG_INFO(ConfigurationManager) << "Configuration version: " << version << ".";

		bool result = true;

		for (ConfigurationBase* installed : m_Installed)
		{
			if (!installed->loadConfiguration(configuration_root, version))
			{
				PS_LOG_INFO(ConfigurationManager) << "Failed to load configuration.";

				return false;
			}
		}

		PS_LOG_INFO(ConfigurationManager) << "Load successful.";

		return result;
	}

	void ConfigurationManager::slotSaveConfiguration()
	{
		save();
	}

	void ConfigurationManager::slotLoadConfiguration()
	{
		load() || save();
	}

}; // namespace Pitstop