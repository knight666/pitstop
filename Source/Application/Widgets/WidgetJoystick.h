#pragma once

#include <Core/Main.h>

#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

#include "Input/RawInput/RawInputJoystick.h"

#include "ui_WidgetJoystick.h"

namespace Pitstop {

	class WidgetJoystick
		: public QWidget
	{

		Q_OBJECT

	public:

		WidgetJoystick(QSharedPointer<RawInputJoystick> joystick, QWidget* parent = nullptr);
		~WidgetJoystick();

		QSharedPointer<RawInputJoystick> getJoystick() const { return m_Joystick; }

	public slots:

		void slotJoystickPropertyChanged();
		void slotJoystickConnected(RawInputJoystick& joystick, bool connected);

		void on_btnDetails_pressed();

	private:

		Ui_JoystickForm m_Form;
		QSharedPointer<RawInputJoystick> m_Joystick;

	}; // class WidgetJoystick

	typedef QSharedPointer<WidgetJoystick> WidgetJoystickPtr;

}; // namespace Pitstop