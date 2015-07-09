#pragma once

#include "Base/Main.h"

namespace Pitstop {

	static const size_t CONFIGURATION_VERSION = 2;

	class ConfigurationBase;

	class ConfigurationManager
		: public QObject
	{

		Q_OBJECT

	public:

		ConfigurationManager();
		~ConfigurationManager();

		void install(ConfigurationBase& configuration);
		void uninstall(ConfigurationBase& configuration);

		bool save();
		bool load();

	public slots:

		void slotSaveConfiguration();
		void slotLoadConfiguration();

	private:

		QVector<ConfigurationBase*> m_Installed;

	}; // class ConfigurationManager

}; // namespace Pitstop