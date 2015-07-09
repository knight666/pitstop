#pragma once

#include "Base/Main.h"
#include "Serialization/ConfigurationBase.h"
#include "Serialization/ConfigurationEventDispatcher.h"

namespace Pitstop {

	static const size_t CONFIGURATION_VERSION = 2;

	class ConfigurationManager
		: public QObject
	{

		Q_OBJECT

	public:

		ConfigurationManager();
		~ConfigurationManager();

		void addEventDispatcher(ConfigurationEventDispatcher& eventDispatcher);
		void removeEventDispatcher(ConfigurationEventDispatcher& eventDispatcher);

		void install(ConfigurationBase& configuration);
		void uninstall(ConfigurationBase& configuration);

		bool save();
		bool load();

	public slots:

		void slotSaveConfiguration();
		void slotLoadConfiguration();

	private:

		QVector<ConfigurationEventDispatcher*> m_EventDispatchers;
		QVector<ConfigurationBase*> m_Installed;

	}; // class ConfigurationManager

}; // namespace Pitstop