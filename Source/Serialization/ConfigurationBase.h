#pragma once

#include "Base/Main.h"
#include "Serialization/ISerializable.h"
#include "Serialization/ConfigurationManager.h"

namespace Pitstop {

	class ConfigurationBase
		: public QObject
		, public ISerializable
	{

		Q_OBJECT

	public:

		ConfigurationBase(ConfigurationManager& manager, const QString& name);
		virtual ~ConfigurationBase();

		bool saveConfiguration(QJsonObject& target, size_t version);
		bool loadConfiguration(const QJsonObject& source, size_t version);

		virtual bool serialize(QJsonObject& target, size_t version = SERIALIZATION_VERSION) = 0;
		virtual bool deserialize(const QJsonObject& source, size_t version = SERIALIZATION_VERSION) = 0;

	signals:

		void signalSaveConfiguration();
		void signalLoadConfiguration();

	protected:

		ConfigurationManager& m_ConfigurationManager;
		QString m_ConfigurationName;

	}; // class ConfigurationBase

}; // namespace Pitstop