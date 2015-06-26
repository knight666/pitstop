#pragma once

#include "Base/Main.h"

#include <QtWidgets/QMainWindow>

#include "ui_MainWindow.h"

namespace Pitstop {

	class MainWindow
		: public QMainWindow
	{

	public:

		MainWindow(QWidget* parent = nullptr, Qt::WindowFlags flags = 0);
		~MainWindow();

	private:

		Ui_MainWindow m_Form;

	}; // class MainWindow

}; // namespace Pitstop