#include "Application/Widgets/WidgetController.h"

#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>

#include "Input/XInput/XInputDevice.h"

namespace Pitstop {

	WidgetController::WidgetController(QWidget* parent /*= nullptr*/)
		: QWidget(parent)
		, m_Icon(":/Icons/Resources/ControllerInverted.png")
	{
	}

	WidgetController::~WidgetController()
	{
	}

	void WidgetController::setDevice(QSharedPointer<XInputDevice> device)
	{
		m_Device = device;
	}

	void WidgetController::paintEvent(QPaintEvent* event)
	{
		QPainter painter_icon(&m_Icon);
		painter_icon.setCompositionMode(QPainter::CompositionMode_SourceIn);
		painter_icon.fillRect(m_Icon.rect(), qRgb(0, 255, 0));
		painter_icon.end();

		QRect icon_rect = m_Icon.rect();
		icon_rect.moveLeft((event->rect().width() - m_Icon.rect().width()) / 2);
		icon_rect.moveTop((event->rect().height() - m_Icon.rect().height()) / 2);

		QPainter painter(this);
		painter.drawPixmap(icon_rect, m_Icon);
	}

}; // namespace Pitstop