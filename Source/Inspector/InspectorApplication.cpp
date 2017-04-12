#include "InspectorApplication.h"

namespace Pitstop {

	InspectorApplication::InspectorApplication(int& argc, char** argv, int flags /*= ApplicationFlags*/)
		: QApplication(argc, argv, flags)
	{
		m_MainWindowForm.setupUi(&m_MainWindow);
	}

	InspectorApplication::~InspectorApplication()
	{
	}

	int InspectorApplication::run()
	{
		m_MainWindow.show();

		return exec();
	}

	bool InspectorApplication::nativeEventFilter(const QByteArray& eventType, void* message, long* result)
	{
		if (eventType != "windows_generic_MSG")
		{
			return false;
		}

		return false;
	}

}; // namespace Pitstop