#pragma once

#include "Base/Main.h"

#include <QtWidgets/QWidget>

#include "ui_WidgetControllerList.h"

namespace Pitstop {

	class WidgetControllerList
		: public QWidget
	{

		Q_OBJECT

	public:

		WidgetControllerList(QWidget* parent = nullptr);
		~WidgetControllerList();

	private:

		void setControllerColor(size_t index, QRgb color);

	private:

		Ui_ControllerListForm m_Form;

	}; // class WidgetControllerList

}; // namespace Pitstop