#include "Serialization/ConfigurationBase.h"

namespace Pitstop {

	ConfigurationBase::ConfigurationBase(ConfigurationManager& manager, const QString& name)
		: m_ConfigurationManager(manager)
		, m_ConfigurationName(name)
	{
		m_ConfigurationManager.install(*this);
	}

	ConfigurationBase::~ConfigurationBase()
	{
		m_ConfigurationManager.uninstall(*this);
	}

	bool ConfigurationBase::saveConfiguration(QJsonObject& target, size_t version)
	{
		QJsonObject configuration_object;
		if (!serialize(configuration_object, version))
		{
			PS_LOG_ERROR(ConfigurationManager) << "Failed to save \"" << m_ConfigurationName << "\" to configuration.";

			return false;
		}
		target[m_ConfigurationName] = configuration_object;

		return true;
	}

	bool ConfigurationBase::loadConfiguration(const QJsonObject& source, size_t version)
	{
		QJsonObject configuration_object = source[m_ConfigurationName].toObject();
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