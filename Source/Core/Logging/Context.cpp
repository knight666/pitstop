#include "Context.h"

#include <Core/Logging/Logger.h>

namespace Pitstop {

	Context::Context(Levels level, const char* module, const char* file, int line)
		: QTextStream(&m_Message)
		, m_Level(level)
		, m_Line(line)
	{
		memset(m_Module, 0, 256);
		strncpy_s(m_Module, module, std::min<size_t>(strlen(module), (size_t)255));

		memset(m_File, 0, _MAX_PATH);
		strncpy_s(m_File, file, std::min<size_t>(strlen(file), (size_t)(_MAX_PATH - 1)));
	}

	Context::~Context()
	{
		m_Message.append('\n');
		QByteArray message = m_Message.toUtf8();

		Logger::get().write(m_Level, m_Module, m_File, m_Line, message.constData());
	}

}; // namespace Pitstop