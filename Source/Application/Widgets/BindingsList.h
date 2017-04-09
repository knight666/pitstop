#pragma once

#include <Core/Main.h>

#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

namespace Ui
{
	class BindingsList;
}

namespace Pitstop {

	class RawInputManager;
	class RawInputJoystick;

	class BindingsList
		: public QWidget
	{
		Q_OBJECT

	public:

		BindingsList(QWidget* parent = nullptr);
		~BindingsList();

		void bind(RawInputJoystick& joystick);

	public slots:

		void slotJoystickInput(RawInputJoystick* joystick, bool processed);

	private:

		Ui::BindingsList* m_Form;
		RawInputJoystick* m_Joystick;
		QHash<QString, QLabel*> m_Labels;

	}; // class BindingsList

}; // namespace Pitstop