#pragma once

#include <Core/Main.h>

#include "Logging/Sink.h"

#define PS_LOG_INFO(_module)   Pitstop::Context(Pitstop::Levels::Information, #_module, __FILE__, __LINE__)
#define PS_LOG_TRACE(_module)  Pitstop::Context(Pitstop::Levels::Trace, #_module, __FILE__, __LINE__)
#define PS_LOG_WARN(_module)   Pitstop::Context(Pitstop::Levels::Warning, #_module, __FILE__, __LINE__)
#define PS_LOG_ERROR(_module)  Pitstop::Context(Pitstop::Levels::Error, #_module, __FILE__, __LINE__)
#define PS_LOG_FATAL(_module)  Pitstop::Context(Pitstop::Levels::Fatal, #_module, __FILE__, __LINE__)

#define PS_LOG_WINDOWS_ERROR " (error \"" << windowsErrorToString(GetLastError()) << "\" code 0x" << QString::number(GetLastError(), 16) << ")";

namespace Pitstop {

	class Logger
		: public QObject
	{

		Q_OBJECT

	public:

		static Logger& get();

		static void initialize();
		static void destroy();

		void addSink(SinkPtr sink);

		void write(Levels level, const char* module, const char* filepath, int line, const char* message);

	private:

		Logger();
		~Logger();

	private:

		static Logger* s_Instance;

	private:

		SinkList m_Sinks;
		QMutex m_Lock;

	};

}; // namespace Pitstop