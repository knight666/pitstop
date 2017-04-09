#pragma once

#include <Core/Main.h>

namespace Pitstop {

	class ConfigurationManager;

	class ConfigurationEventDispatcher
		: public QObject
	{

		Q_OBJECT

	public:

		ConfigurationEventDispatcher(QSharedPointer<ConfigurationManager> configuration);
		virtual ~ConfigurationEventDispatcher();

	signals:

		void signalSaveConfiguration();
		void signalLoadConfiguration();

	protected:

		QSharedPointer<ConfigurationManager> m_Configuration;

	}; // class ConfigurationEventDispatcher

}; // namespace Pitstop