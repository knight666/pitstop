#include "Application/MainWindow.h"

#include "Input/XInput/XInputManager.h"
#include "Input/RawInputManager.h"

namespace Pitstop {

	MainWindow::MainWindow(
			RawInputManager& rawInput,
			UsbController& usb,
			VirtualInputManager& virtualInput,
			QSharedPointer<XInputManager> xinput)
		: QMainWindow(nullptr, 0)
		, m_RawInput(rawInput)
		, m_Usb(usb)
		, m_VirtualInput(virtualInput)
	{
		m_Form.setupUi(this);

		m_Form.tabJoysticks->setup(rawInput);
		m_Form.tabDevices->setup(rawInput, usb, virtualInput);

		m_Form.wdgControllerList->initialize(xinput);
	}

	MainWindow::~MainWindow()
	{
	}

	void MainWindow::bindJoystick(RawInputJoystick& joystick)
	{
		m_Form.bindingsList->bind(joystick);
	}

}; // namespace Pitstop