#include "Application/Widgets/WidgetControllerList.h"

#include <QtGui/QPainter>

#include "Application/Widgets/WidgetController.h"
#include "Input/XInput/XInputManager.h"

namespace Pitstop {

	WidgetControllerList::WidgetControllerList(QWidget* parent /*= nullptr*/)
		: QWidget(parent)
	{
		m_Form.setupUi(this);
	}

	WidgetControllerList::~WidgetControllerList()
	{
	}

	void WidgetControllerList::initialize(QSharedPointer<XInputManager> xinput)
	{
		WidgetController* controller_list[XUSER_MAX_COUNT] = {
			m_Form.wdgController1,
			m_Form.wdgController2,
			m_Form.wdgController3,
			m_Form.wdgController4
		};

		for (size_t i = 0; i < XUSER_MAX_COUNT; ++i)
		{
			controller_list[i]->setDevice(
				xinput->getDeviceByIndex(i));
		}
	}

}; // namespace Pitstop