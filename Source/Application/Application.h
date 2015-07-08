#pragma once

#include "Base/Main.h"

#include <QtCore/QAbstractNativeEventFilter>
#include <QtWidgets/QApplication>

namespace Pitstop {

	class MainWindow;
	class RawInputManager;
	class UsbController;
	class VirtualInputManager;

	class Application
		: public QApplication
		, public QAbstractNativeEventFilter
	{

	public:

		Application(int& argc, char** argv, int flags = ApplicationFlags);
		~Application();

		static Application& get();

		bool saveConfiguration();
		bool loadConfiguration();

		int run();

	private:

		virtual bool nativeEventFilter(const QByteArray& eventType, void* message, long* result) override;

	private:

		static Application* s_Instance;
		RawInputManager* m_RawInput;
		UsbController* m_UsbController;
		VirtualInputManager* m_VirtualInput;
		MainWindow* m_MainWindow;

	}; // class Application

}; // namespace Pitstop