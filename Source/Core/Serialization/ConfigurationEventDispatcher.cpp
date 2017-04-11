#include "ConfigurationEventDispatcher.h"

#include <Core/Serialization/ConfigurationManager.h>

namespace Pitstop {

	ConfigurationEventDispatcher::ConfigurationEventDispatcher(QSharedPointer<ConfigurationManager> configuration)
		: m_Configuration(configuration)
	{
		m_Configuration->addEventDispatcher(*this);
	}

	ConfigurationEventDispatcher::~ConfigurationEventDispatcher()
	{
		m_Configuration->removeEventDispatcher(*this);
	}

}; // namespace Pitstop