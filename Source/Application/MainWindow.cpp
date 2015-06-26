#include "Application/MainWindow.h"

namespace Pitstop {

	MainWindow::MainWindow(QWidget* parent /*= nullptr*/, Qt::WindowFlags flags /*= 0*/)
		: QMainWindow(parent, flags)
	{
		m_Form.setupUi(this);
	}

	MainWindow::~MainWindow()
	{
	}

}; // namespace Pitstop