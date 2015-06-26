#pragma once

#include "Base/Main.h"

#include <QtWidgets/QApplication>

namespace Pitstop {

	class Application
		: public QApplication
	{

	public:

		Application(int& argc, char** argv, int flags = ApplicationFlags);
		~Application();

		int run();

	}; // class Application

}; // namespace Pitstop