#pragma once

#include "Base/Main.h"

#include <QtWidgets/QWidget>

#include "ui_WidgetControllerList.h"

namespace Pitstop {

	class XInputManager;

	class WidgetControllerList
		: public QWidget
	{

		Q_OBJECT

	public:

		WidgetControllerList(QWidget* parent = nullptr);
		~WidgetControllerList();

		void initialize(QSharedPointer<XInputManager> xinput);

	private:

		Ui_ControllerListForm m_Form;

	}; // class WidgetControllerList

}; // namespace Pitstop