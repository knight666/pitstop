#include "Logger.h"

#include <time.h>

#include <Core/Logging/SinkFile.h>

namespace Pitstop {

	Logger* Logger::s_Instance = nullptr;

	Logger::Logger()
	{
		addSink(SinkPtr(new SinkFile("Pitstop.log")));
	}

	Logger::~Logger()
	{
		m_Sinks.clear();
	}

	Logger& Logger::get()
	{
		return *s_Instance;
	}

	void Logger::initialize()
	{
		s_Instance = new Logger();
	}

	void Logger::destroy()
	{
		PS_LOG_INFO(Logger) << "Closing log.";

		delete s_Instance;
		s_Instance = nullptr;
	}

	void Logger::addSink(SinkPtr a_Sink)
	{
		m_Sinks.push_back(a_Sink);
	}

	void Logger::write(Levels level, const char* module, const char* filepath, int line, const char* message)
	{
		QMutexLocker locker(&m_Lock);

		time_t current_time = time(0);
		struct tm* now = localtime(&current_time);

		char timestamp[32] = { 0 };
		_snprintf(
			timestamp,
			31,
			"%02d:%02d:%02d",
			now->tm_hour,
			now->tm_min,
			now->tm_sec);

		const char* path_last_slash = strrchr(filepath, '\\');
		const char* filename = (path_last_slash != nullptr) ? (path_last_slash + 1) : filepath;

		for (SinkPtr& sink : m_Sinks)
		{
			sink->Write(level, module, timestamp, filename, line, message);
		}
	}

}; // namespace Pitstop