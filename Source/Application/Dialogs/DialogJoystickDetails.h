#pragma once

#include "Base/Main.h"

#include <QtWidgets/QDialog>

#include "ui_DialogJoystickDetails.h"

namespace Pitstop {

	class DialogJoystickDetails
		: public QDialog
	{

		Q_OBJECT

	public:

		DialogJoystickDetails(QWidget* parent = nullptr, Qt::WindowFlags flags = 0);
		~DialogJoystickDetails();

	private:

		Ui_DialogJoystickDetails m_Form;

	}; // class DialogJoystickDetails

}; // namespace Pitstop