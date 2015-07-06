#include "Logging/Logger.h"

#include <time.h>

#include "Logging/SinkFile.h"

namespace Logging {

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
		delete s_Instance;
		s_Instance = nullptr;
	}

	void Logger::addSink(Logging::SinkPtr a_Sink)
	{
		m_Sinks.push_back(a_Sink);
	}

	void Logger::write(Logging::Levels level, const char* module, const char* filename, int line, const char* message)
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

		for (Logging::SinkPtr& sink : m_Sinks)
		{
			sink->Write(level, module, timestamp, filename, line, message);
		}
	}

}; // namespace Logging