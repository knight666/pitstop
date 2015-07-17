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

	public slots:

		void on_wdgController1_pressed();
		void on_wdgController2_pressed();
		void on_wdgController3_pressed();
		void on_wdgController4_pressed();

	private:

		void openControllerDialog(size_t index);

	private:

		Ui_ControllerListForm m_Form;
		QSharedPointer<XInputManager> m_Xinput;

	}; // class WidgetControllerList

}; // namespace Pitstop