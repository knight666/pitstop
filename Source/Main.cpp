#include "Base/Main.h"

#include "Application/Application.h"

int main(int argc, char** argv)
{
	g_ApplicationDir = QDir::homePath() + "/Pitstop";
	g_ApplicationDir.mkpath(".");

	Logging::Logger::initialize();

	Pitstop::Application application(argc, argv);
	int result = application.run();

	Logging::Logger::destroy();

	return result;
}