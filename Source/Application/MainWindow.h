#pragma once

#include "Base/Main.h"

#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>

#include "ui_MainWindow.h"

namespace Pitstop {

	class InputProcessorBase;
	class RawInputJoystick;

	class MainWindow
		: public QMainWindow
	{

		Q_OBJECT

	public:

		MainWindow();
		~MainWindow();

		void bindJoystick(RawInputJoystick& joystick);
		void updateBindings();

	private:

		Ui_MainWindow m_Form;

	}; // class MainWindow

}; // namespace Pitstop