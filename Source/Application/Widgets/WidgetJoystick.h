#pragma once

#include "Base/Main.h"

#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

#include "Input/RawInputJoystick.h"

#include "ui_WidgetJoystick.h"

namespace Pitstop {

	class WidgetJoystick
		: public QWidget
	{

		Q_OBJECT

	public:

		WidgetJoystick(RawInputJoystickPtr joystick, QWidget* parent = nullptr);
		~WidgetJoystick();

		RawInputJoystickPtr getJoystick() const { return m_Joystick; }

	public slots:

		void slotJoystickPropertyChanged();
		void slotJoystickConnected(RawInputJoystick& joystick, bool connected);

		void on_btnDetails_pressed();

	private:

		Ui_JoystickForm m_Form;
		RawInputJoystickPtr m_Joystick;

	}; // class WidgetJoystick

	typedef QSharedPointer<WidgetJoystick> WidgetJoystickPtr;

}; // namespace Pitstop