#pragma once

#include "Base/Main.h"
#include "Input/RawInputJoystick.h"
#include "Application/Widgets/WidgetJoystick.h"

#include <QtWidgets/QWidget>

#include "ui_TabJoysticks.h"

namespace Pitstop {

	class TabJoysticks
		: public QWidget
	{

		Q_OBJECT

	public:

		TabJoysticks(QWidget* parent = nullptr, Qt::WindowFlags flags = 0);
		~TabJoysticks();

		void setup(RawInputManager& rawInput);

	public slots:

		void slotJoystickCreated(RawInputJoystickPtr joystick);

	private:

		Ui_TabJoysticks m_Form;
		QHash<QString, WidgetJoystickPtr> m_Joysticks;

	}; // class TabJoysticks

	typedef QSharedPointer<TabJoysticks> TabJoysticksPtr;

}; // namespace Pitstop