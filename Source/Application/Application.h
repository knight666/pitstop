#pragma once

#include "Base/Main.h"

#include <QtCore/QAbstractNativeEventFilter>
#include <QtWidgets/QApplication>

namespace Pitstop {

	class ConfigurationManager;
	class ContainerManager;
	class MainWindow;
	class RawInputManager;
	class UsbController;
	class VirtualInputManager;

	class Application
		: public QApplication
		, public QAbstractNativeEventFilter
	{

		Q_OBJECT

	public:

		Application(int& argc, char** argv, int flags = ApplicationFlags);
		~Application();

		static Application& get();

		int run();

	private:

		virtual bool nativeEventFilter(const QByteArray& eventType, void* message, long* result) override;

	private:

		static Application* s_Instance;
		bool m_Quit;
		QSharedPointer<ConfigurationManager> m_Configuration;
		QSharedPointer<ContainerManager> m_Containers;
		RawInputManager* m_RawInput;
		UsbController* m_UsbController;
		VirtualInputManager* m_VirtualInput;
		MainWindow* m_MainWindow;

	}; // class Application

}; // namespace Pitstop