#pragma once

#include "Base/Main.h"

#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>

#include "ui_MainWindow.h"

namespace Pitstop {

	class InputProcessorBase;
	class RawInputJoystick;
	class RawInputManager;

	class MainWindow
		: public QMainWindow
	{

	public:

		MainWindow(RawInputManager& rawInput);
		~MainWindow();

		void bindJoystick(RawInputJoystick& joystick);
		void updateBindings();

	private:

		RawInputManager& m_RawInput;
		Ui_MainWindow m_Form;

	}; // class MainWindow

}; // namespace Pitstop