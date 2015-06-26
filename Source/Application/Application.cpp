#include "Application.h"

namespace Pitstop {

	Application::Application(int& argc, char** argv, int flags /*= ApplicationFlags*/)
		: QApplication(argc, argv, flags)
	{
	}

	Application::~Application()
	{
	}

	int Application::run()
	{
		return 0;
	}

}; // namespace Pitstop