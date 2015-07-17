#include "Application/Widgets/WidgetController.h"

#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>

#include "Input/XInput/XInputDevice.h"

namespace Pitstop {

	WidgetController::WidgetController(QWidget* parent /*= nullptr*/)
		: QWidget(parent)
		, m_Icon(":/Icons/Resources/ControllerInverted.png")
		, m_IconDirty(true)
	{
	}

	WidgetController::~WidgetController()
	{
		disconnect(
			this, SLOT(slotDeviceChanged()));
	}

	void WidgetController::setDevice(QSharedPointer<XInputDevice> device)
	{
		connect(
			device.data(), SIGNAL(signalChanged()),
			this, SLOT(slotDeviceChanged()));

		m_Device = device;

		slotDeviceChanged();
	}

	void WidgetController::slotDeviceChanged()
	{
		setEnabled(m_Device->isConnected());

		m_IconDirty = true;

		update();
	}

	void WidgetController::paintEvent(QPaintEvent* event)
	{
		if (m_IconDirty)
		{
			QPainter painter_icon(&m_Icon);
			painter_icon.setCompositionMode(QPainter::CompositionMode_SourceIn);

			QRgb color = qRgb(90, 90, 90);

			if (m_Device != nullptr &&
				m_Device->isConnected())
			{
				color = qRgb(0, 255, 0);
			}

			painter_icon.fillRect(m_Icon.rect(), color);
			painter_icon.end();

			m_IconDirty = false;
		}

		QRect icon_rect = m_Icon.rect();
		icon_rect.moveLeft((event->rect().width() - m_Icon.rect().width()) / 2);
		icon_rect.moveTop((event->rect().height() - m_Icon.rect().height()) / 2);

		QPainter painter(this);

		if (m_Device->isActivated())
		{
			painter.setBrush(QBrush(qRgb(255, 255, 0)));

			QPoint circle_center = event->rect().center();
			circle_center.setY(circle_center.y() + 6);

			painter.drawEllipse(circle_center, 24, 24);
		}

		painter.drawPixmap(icon_rect, m_Icon);
	}

}; // namespace Pitstop