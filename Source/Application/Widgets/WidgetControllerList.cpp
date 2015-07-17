#include "Application/Widgets/WidgetControllerList.h"

#include <QtGui/QPainter>

#include "Application/Dialogs/DialogController.h"
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

		m_Xinput = xinput;
	}

	void WidgetControllerList::on_wdgController1_pressed()
	{
		openControllerDialog(0);
	}

	void WidgetControllerList::on_wdgController2_pressed()
	{
		openControllerDialog(1);
	}

	void WidgetControllerList::on_wdgController3_pressed()
	{
		openControllerDialog(2);
	}

	void WidgetControllerList::on_wdgController4_pressed()
	{
		openControllerDialog(3);
	}

	void WidgetControllerList::openControllerDialog(size_t index)
	{
		if (m_Xinput == nullptr)
		{
			return;
		}

		QSharedPointer<XInputDevice> device = m_Xinput->getDeviceByIndex(index);
		if (device == nullptr)
		{
			return;
		}

		DialogController dialog(device);
		dialog.exec();
	}

}; // namespace Pitstop