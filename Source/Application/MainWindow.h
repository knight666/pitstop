#pragma once

#include "Base/Main.h"

#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>

#include "Application/Widgets/WidgetJoystick.h"
#include "ui_MainWindow.h"

namespace Pitstop {

	class InputProcessorBase;
	class RawInputJoystick;
	class RawInputManager;

	class MainWindow
		: public QMainWindow
	{

		Q_OBJECT

	public:

		MainWindow(RawInputManager& rawInput);
		~MainWindow();

		void bindJoystick(RawInputJoystick& joystick);

	public slots:

		void slotJoystickConnected(RawInputJoystickPtr joystick, bool connected);

	private:

		RawInputManager& m_RawInput;
		Ui_MainWindow m_Form;
		QHash<QString, WidgetJoystickPtr> m_JoystickWidgets;

	}; // class MainWindow

}; // namespace Pitstop