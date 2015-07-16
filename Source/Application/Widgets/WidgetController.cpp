#include "Application/Widgets/WidgetController.h"

#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>

#include "Input/XInput/XInputDevice.h"

namespace Pitstop {

	WidgetController::WidgetController(QWidget* parent /*= nullptr*/)
		: QPushButton(parent)
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

		m_IconDirty = true;

		update();
	}

	void WidgetController::slotDeviceChanged()
	{
		m_IconDirty = true;

		update();
	}

	void WidgetController::paintEvent(QPaintEvent* event)
	{
		QPushButton::paintEvent(event);

		if (m_IconDirty)
		{
			QPainter painter_icon(&m_Icon);
			painter_icon.setCompositionMode(QPainter::CompositionMode_SourceIn);

			QRgb color = qRgb(80, 80, 80);

			if (m_Device != nullptr &&
				m_Device->isConnected())
			{
				color = qRgb(35, 176, 33);
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
			painter.setPen(Qt::NoPen);
			painter.setBrush(QBrush(qRgb(255, 173, 12)));

			painter.drawEllipse(
				event->rect().center(),
				28, 28);
		}

		painter.drawPixmap(icon_rect, m_Icon);
	}

}; // namespace Pitstop