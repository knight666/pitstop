#pragma once

#include "Base/Main.h"

#include <QtWidgets/QMainWindow>

#include "ui_MainWindow.h"

namespace Pitstop {

	class RawInputJoystick;
	class RawInputManager;
	class UsbController;
	class VirtualInputManager;

	class MainWindow
		: public QMainWindow
	{

		Q_OBJECT

	public:

		MainWindow(
			RawInputManager& rawInput,
			UsbController& usb,
			VirtualInputManager& virtualInput);
		~MainWindow();

		void bindJoystick(RawInputJoystick& joystick);

	private:

		RawInputManager& m_RawInput;
		UsbController& m_Usb;
		VirtualInputManager& m_VirtualInput;

		Ui_MainWindow m_Form;

	}; // class MainWindow

}; // namespace Pitstop