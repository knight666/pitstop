#pragma once

#include "Base/Main.h"
#include "Serialization/ISerializable.h"
#include "Serialization/ConfigurationEventDispatcher.h"

namespace Pitstop {

	class ConfigurationManager;

	class ConfigurationBase
		: public ConfigurationEventDispatcher
		, public ISerializable
	{

		Q_OBJECT

	public:

		ConfigurationBase(QSharedPointer<ConfigurationManager> configuration, const QString& name);
		virtual ~ConfigurationBase();

		bool saveConfiguration(QJsonObject& root, size_t version);
		bool loadConfiguration(const QJsonObject& root, size_t version);

	protected:

		QString m_ConfigurationName;

	}; // class ConfigurationBase

}; // namespace Pitstop