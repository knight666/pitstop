#pragma once

#include "Base/Main.h"
#include "Input/RawInputJoystick.h"
#include "Application/Widgets/WidgetJoystick.h"

#include "ui_TabDevices.h"

namespace Pitstop {

	class TabDevices
		: public QWidget
	{

		Q_OBJECT

	public:

		TabDevices(QWidget* parent = nullptr, Qt::WindowFlags flags = 0);
		~TabDevices();

		void setup(RawInputManager& rawInput);

	private:

		Ui_TabDevices m_Form;

	}; // class TabDevices

	typedef QSharedPointer<TabDevices> TabDevicesPtr;

}; // namespace Pitstop