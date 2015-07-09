#include "Serialization/ConfigurationBase.h"

#include "Serialization/ConfigurationManager.h"

namespace Pitstop {

	ConfigurationBase::ConfigurationBase(QSharedPointer<ConfigurationManager> manager, const QString& name)
		: m_Configuration(manager)
		, m_ConfigurationName(name)
	{
		m_Configuration->install(*this);
	}

	ConfigurationBase::~ConfigurationBase()
	{
		m_Configuration->uninstall(*this);
	}

	bool ConfigurationBase::saveConfiguration(QJsonObject& root, size_t version)
	{
		QJsonObject configuration_object;
		if (!serialize(configuration_object, version))
		{
			PS_LOG_ERROR(ConfigurationManager) << "Failed to save \"" << m_ConfigurationName << "\" to configuration.";

			return false;
		}
		root[m_ConfigurationName] = configuration_object;

		return true;
	}

	bool ConfigurationBase::loadConfiguration(const QJsonObject& root, size_t version)
	{
		QJsonObject configuration_object = root[m_ConfigurationName].toObject();
		if (configuration_object.isEmpty())
		{
			PS_LOG_ERROR(ConfigurationManager) << "Missing \"" << m_ConfigurationName << "\" while loading configuration.";

			return false;
		}

		if (!deserialize(configuration_object, version))
		{
			PS_LOG_ERROR(ConfigurationManager) << "Failed to load \"" << m_ConfigurationName << "\" from configuration.";

			return false;
		}

		return true;
	}

}; // namespace Pitstop