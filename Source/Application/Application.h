#pragma once

#include "Base/Main.h"

#include "Input/RawInputManager.h"

#include <QtWidgets/QApplication>

namespace Pitstop {

	class Application
		: public QApplication
	{

	public:

		Application(int& argc, char** argv, int flags = ApplicationFlags);
		~Application();

		int run();

	private:

		RawInputManager* m_RawInput;

	}; // class Application

}; // namespace Pitstop