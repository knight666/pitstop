#include "Serialization/ConfigurationManager.h"

#include "Serialization/ConfigurationBase.h"

namespace Pitstop {

	ConfigurationManager::ConfigurationManager()
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

		connect(
			&configuration, SIGNAL(signalSaveConfiguration()),
			this, SLOT(slotSaveConfiguration()));

		connect(
			&configuration, SIGNAL(signalLoadConfiguration()),
			this, SLOT(slotLoadConfiguration()));

		m_Installed.push_back(&configuration);
	}

	void ConfigurationManager::uninstall(ConfigurationBase& configuration)
	{
		for (QVector<ConfigurationBase*>::iterator it = m_Installed.begin(); it != m_Installed.end(); ++it)
		{
			if (*it == &configuration)
			{
				disconnect(
					&configuration, SIGNAL(signalSaveConfiguration()),
					this, SLOT(slotSaveConfiguration()));

				disconnect(
					&configuration, SIGNAL(signalLoadConfiguration()),
					this, SLOT(slotLoadConfiguration()));

				m_Installed.erase(it);

				return;
			}
		}

		PS_LOG_WARN(ConfigurationManager) << "Configuration was already uninstalled.";
	}

	bool ConfigurationManager::save()
	{
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
		PS_LOG_INFO(ConfigurationManager) << "Loading configuration from disk.";

		QDir app_directory(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));

		QFile configuration_file(app_directory.absoluteFilePath("configuration.json"));
		if (!configuration_file.open(QIODevice::ReadOnly))
		{
			PS_LOG_ERROR(ConfigurationManager) << "Failed to open \"configuration.json\" for reading.";

			return false;
		}

		QJsonDocument configuration_document = QJsonDocument::fromJson(configuration_file.readAll());
		configuration_file.close();

		if (configuration_document.isNull())
		{
			PS_LOG_ERROR(ConfigurationManager) << "Failed to parse configuration document.";

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