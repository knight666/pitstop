#include "Logging/SinkFile.h"

namespace Logging {

	SinkFile::SinkFile(const QString& filename)
		: m_File(QDir::current().absoluteFilePath(filename))
	{
		// clear file

		if (m_File.open(QIODevice::WriteOnly))
		{
			m_File.close();
		}
	}

	void SinkFile::Write(Levels level, const char* module, const char* timestamp, const char* filename, int line, const char* message)
	{
		if (m_File.open(QIODevice::WriteOnly | QIODevice::Append))
		{
			char logmessage_formatted[1024] = { 0 };
			_snprintf(
				logmessage_formatted,
				1023,
				"(%s) [%s] [%s] ",
				timestamp,
				LevelToString(level),
				module);
			m_File.write(logmessage_formatted);

			m_File.write(message);

			m_File.close();
		}
	}

}; // namespace Logging