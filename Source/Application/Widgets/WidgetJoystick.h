#pragma once

#include "Base/Main.h"

#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

class Ui_JoystickForm;

namespace Pitstop {

	class RawInputJoystick;

	class WidgetJoystick
		: public QWidget
	{
		Q_OBJECT

	public:

		WidgetJoystick(QWidget* parent = nullptr);
		~WidgetJoystick();

		void bind(RawInputJoystick& joystick);

	private:

		Ui_JoystickForm* m_Form;
		RawInputJoystick* m_Joystick;

	}; // class WidgetJoystick

}; // namespace Pitstop