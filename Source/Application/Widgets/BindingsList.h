#pragma once

#include "Base/Main.h"

#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

namespace Ui
{
	class BindingsList;
}

namespace Pitstop {

	class InputProcessorBase;

	class BindingsList
		: public QWidget
	{
		Q_OBJECT

	public:

		BindingsList(QWidget* parent = nullptr);
		~BindingsList();

		void bind(InputProcessorBase& processor);
		void update();

	private:

		Ui::BindingsList* m_Form;
		InputProcessorBase* m_Processor;
		QHash<QString, QLabel*> m_Labels;

	}; // class BindingsList

}; // namespace Pitstop