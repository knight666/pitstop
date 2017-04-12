#pragma once

#include <Core/Main.h>

#include <QtCore/QAbstractNativeEventFilter>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>

#include "ui_InspectorWindow.h"

namespace Pitstop {

	class InspectorApplication
		: public QApplication
		, public QAbstractNativeEventFilter
	{

		Q_OBJECT

	public:

		InspectorApplication(int& argc, char** argv, int flags = ApplicationFlags);
		~InspectorApplication();

		int run();

	private:

		virtual bool nativeEventFilter(const QByteArray& eventType, void* message, long* result) override;

	private:

		Ui_InspectorWindow m_MainWindowForm;
		QMainWindow m_MainWindow;

	};

}; // namespace Pitstop