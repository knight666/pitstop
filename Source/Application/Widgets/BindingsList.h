#pragma once

#include "Base/Main.h"

#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

namespace Ui
{
	class BindingsList;
}

namespace Pitstop {

	class RawInputJoystick;

	class BindingsList
		: public QWidget
	{
		Q_OBJECT

	public:

		BindingsList(QWidget* parent = nullptr);
		~BindingsList();

		void bind(RawInputJoystick& joystick);
		void update();

	private:

		Ui::BindingsList* m_Form;
		RawInputJoystick* m_Joystick;
		QHash<QString, QLabel*> m_Labels;

	}; // class BindingsList

}; // namespace Pitstop