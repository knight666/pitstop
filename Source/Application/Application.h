#pragma once

#include "Base/Main.h"

#include <QtCore/QAbstractNativeEventFilter>
#include <QtWidgets/QApplication>

namespace Pitstop {

	class MainWindow;
	class RawInputManager;
	class UsbController;

	class Application
		: public QApplication
		, public QAbstractNativeEventFilter
	{

	public:

		Application(int& argc, char** argv, int flags = ApplicationFlags);
		~Application();

		int run();

	private:

		virtual bool nativeEventFilter(const QByteArray& eventType, void* message, long* result) override;

	private:

		MainWindow* m_MainWindow;
		RawInputManager* m_RawInput;
		UsbController* m_UsbController;

	}; // class Application

}; // namespace Pitstop