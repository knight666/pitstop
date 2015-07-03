#pragma once

#include "Base/Main.h"
#include "Logging/Levels.h"

namespace Logging {

	class Context
		: public QTextStream
	{

	public:

		Context(Levels level, const char* module, const char* file, int line);
		~Context();

	private:

		Levels m_Level;
		char m_Module[256];
		char m_File[_MAX_PATH];
		int m_Line;
		QString m_Message;

	};

}; // namespace Logging