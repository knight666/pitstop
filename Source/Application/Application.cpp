#include "Application/Application.h"

namespace Pitstop {

	Application::Application(int& argc, char** argv, int flags /*= ApplicationFlags*/)
		: QApplication(argc, argv, flags)
	{
		m_RawInput = new RawInputManager();
	}

	Application::~Application()
	{
		delete m_RawInput;
	}

	int Application::run()
	{
		if (!m_RawInput->initialize())
		{
			return false;
		}

		return exec();
	}

}; // namespace Pitstop