#pragma once

#include "Base/Main.h"

#include <QtWidgets/QDialog>

#include "ui_DialogController.h"

namespace Pitstop {

	class XInputDevice;

	class DialogController
		: public QDialog
	{

		Q_OBJECT

	public:

		DialogController(QSharedPointer<XInputDevice> device);
		~DialogController();

	public slots:

		void slotDeviceChanged();

	private:

		Ui_DialogController m_Form;
		QSharedPointer<XInputDevice> m_Device;

	}; // class DialogController

}; // namespace Pitstop