#pragma once

#include "Base/Main.h"

#include "Logging/Sink.h"

#define PS_LOG_INFO(_module)   Logging::Context(Logging::Levels::Information, #_module, __FILE__, __LINE__)
#define PS_LOG_TRACE(_module)  Logging::Context(Logging::Levels::Trace, #_module, __FILE__, __LINE__)
#define PS_LOG_WARN(_module)   Logging::Context(Logging::Levels::Warning, #_module, __FILE__, __LINE__)
#define PS_LOG_ERROR(_module)  Logging::Context(Logging::Levels::Error, #_module, __FILE__, __LINE__)
#define PS_LOG_FATAL(_module)  Logging::Context(Logging::Levels::Fatal, #_module, __FILE__, __LINE__)

namespace Logging {

	class Logger
		: public QObject
	{

		Q_OBJECT

	public:

		static Logger& get();

		static void initialize();
		static void destroy();

		void synchronize();

		void addSink(SinkPtr sink);

		void write(Levels level, const char* module, const char* filename, int line, const char* message);

	private:

		Logger();
		~Logger();

	private:

		static Logger* s_Instance;

	private:

		SinkList m_Sinks;
		QMutex m_Lock;

		struct CachedMessage
		{
			QByteArray timestamp;
			Levels level;
			QByteArray module;
			QByteArray filename;
			int line;
			QByteArray message;
		};
		QVector<CachedMessage> m_Cached;
		bool m_Synchronized;

	};

}; // namespace Logging