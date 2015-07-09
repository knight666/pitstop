#pragma once

#include "Base/Main.h"
#include "Serialization/ISerializable.h"

namespace Pitstop {

	class ConfigurationManager;

	class ConfigurationBase
		: public QObject
		, public ISerializable
	{

		Q_OBJECT

	public:

		ConfigurationBase(QSharedPointer<ConfigurationManager> configuration, const QString& name);
		virtual ~ConfigurationBase();

		bool saveConfiguration(QJsonObject& root, size_t version);
		bool loadConfiguration(const QJsonObject& root, size_t version);

		virtual bool serialize(QJsonObject& target, size_t version = SERIALIZATION_VERSION) = 0;
		virtual bool deserialize(const QJsonObject& source, size_t version = SERIALIZATION_VERSION) = 0;

	signals:

		void signalSaveConfiguration();
		void signalLoadConfiguration();

	protected:

		QSharedPointer<ConfigurationManager> m_Configuration;
		QString m_ConfigurationName;

	}; // class ConfigurationBase

}; // namespace Pitstop