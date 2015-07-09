#pragma once

#include "Base/Main.h"
#include "Serialization/ConfigurationBase.h"
#include "Serialization/ConfigurationEventDispatcher.h"

namespace Pitstop {

	static const size_t CONFIGURATION_VERSION = 4;

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

		class ScopedLock
		{

		public:

			ScopedLock(bool& locked) : m_Locked(locked) { m_Locked = true; }
			~ScopedLock() { m_Locked = false; }

		private:

			bool& m_Locked;

		};

		bool m_Locked;
		QVector<ConfigurationEventDispatcher*> m_EventDispatchers;
		QVector<ConfigurationBase*> m_Installed;

	}; // class ConfigurationManager

}; // namespace Pitstop