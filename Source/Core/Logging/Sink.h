#pragma once

#include <Core/Main.h>
#include <Core/Logging/Levels.h>

namespace Pitstop {

	class Sink
	{

	public:

		virtual ~Sink() { }

		virtual void Write(Levels level, const char* module, const char* timestamp, const char* filename, int line, const char* message) = 0;

	};

	typedef QSharedPointer<Sink> SinkPtr;
	typedef QVector<SinkPtr> SinkList;

}; // namespace Pitstop