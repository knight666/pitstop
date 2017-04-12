#include "InspectorApplication.h"

int main(int argc, char** argv)
{
	Pitstop::Logger::initialize();

	Pitstop::InspectorApplication application(argc, argv);
	int result = application.run();

	Pitstop::Logger::destroy();

	return result;
}