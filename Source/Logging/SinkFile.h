#pragma once

#include "Base/Main.h"
#include "Logging/Sink.h"

namespace Logging {

	class SinkFile
		: public Sink
	{

	public:

		SinkFile(const QString& filename);

		virtual void Write(Levels level, const char* module, const char* timestamp, const char* filename, int line, const char* message) override;

	private:

		QFile m_File;

	};

}; // namespace Logging