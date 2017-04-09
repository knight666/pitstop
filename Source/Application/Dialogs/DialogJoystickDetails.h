#pragma once

#include <Core/Main.h>

#include <QtWidgets/QDialog>

#include "ui_DialogJoystickDetails.h"

namespace Pitstop {

	class RawInputJoystick;
	class WidgetJoystick;

	class DialogJoystickDetails
		: public QDialog
	{

		Q_OBJECT

	public:

		DialogJoystickDetails(WidgetJoystick& widget);
		~DialogJoystickDetails();

	private:

		void addRow(const QString& name, const QString& value);

	private:

		Ui_DialogJoystickDetails m_Form;
		QSharedPointer<RawInputJoystick> m_Joystick;

	}; // class DialogJoystickDetails

}; // namespace Pitstop