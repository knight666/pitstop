#include "Application/Application.h"

namespace Pitstop {

	Application::Application(int& argc, char** argv, int flags /*= ApplicationFlags*/)
		: QApplication(argc, argv, flags)
	{
		m_RawInput = new RawInputManager();
	}

	Application::~Application()
	{
	}

	int Application::run()
	{
		m_RawInput->initialize();

		return 0;
	}

}; // namespace Pitstop