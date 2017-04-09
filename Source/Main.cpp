#include <Core/Main.h>

#include "Application/Application.h"

int main(int argc, char** argv)
{
	Pitstop::Application application(argc, argv);

	return application.run();
}