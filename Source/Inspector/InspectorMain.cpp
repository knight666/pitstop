#include "InspectorApplication.h"

int main(int argc, char** argv)
{
	Pitstop::Logger::initialize();

	Pitstop::InspectorApplication* application = new Pitstop::InspectorApplication(argc, argv);
	int result = application->run();
	delete application;

	Pitstop::Logger::destroy();

	return result;
}