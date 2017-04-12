#pragma once

#include <Core/Main.h>

#include <QtCore/QAbstractNativeEventFilter>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>

#include "ui_InspectorWindow.h"

namespace Pitstop {

	class ContainerManager;
	class RawInputJoystick;
	class RawInputManager;

	class InspectorApplication
		: public QApplication
		, public QAbstractNativeEventFilter
	{

		Q_OBJECT

	public:

		InspectorApplication(int& argc, char** argv, int flags = ApplicationFlags);
		~InspectorApplication();

		int run();

	private slots:

		void slotJoystickConnected(QSharedPointer<RawInputJoystick> joystick, bool connected);

	private:

		virtual bool nativeEventFilter(const QByteArray& eventType, void* message, long* result) override;

	private:

		Ui_InspectorWindow m_MainWindowForm;
		QMainWindow m_MainWindow;

		QSharedPointer<ContainerManager> m_Containers;
		QSharedPointer<RawInputManager> m_RawInput;

	};

}; // namespace Pitstop