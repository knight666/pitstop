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

		RawInputManager* m_RawInput;
		UsbController* m_UsbController;
		MainWindow* m_MainWindow;

	}; // class Application

}; // namespace Pitstop