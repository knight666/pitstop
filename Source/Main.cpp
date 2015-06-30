#include "Base/Main.h"

#include "Application/Application.h"

int main(int argc, char** argv)
{
	Pitstop::Application application(argc, argv);

	g_ApplicationDir.mkpath(".");

	return application.run();
}