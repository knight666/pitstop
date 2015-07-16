#include "Application/Widgets/WidgetDualAxis.h"

#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>

namespace Pitstop {

	WidgetDualAxis::WidgetDualAxis(QWidget* parent /*= nullptr*/)
		: m_HorizontalValue(0.0f)
		, m_VerticalValue(0.0f)
		, m_Range(1.0f)
		, m_Treshold(0.0f)
		
	{
	}

	WidgetDualAxis::~WidgetDualAxis()
	{
	}

	void WidgetDualAxis::setValue(float horizontal, float vertical)
	{
		horizontal = qMax(-m_Range, qMin(horizontal, m_Range));
		vertical = qMax(-m_Range, qMin(vertical, m_Range));

		if (horizontal != m_HorizontalValue ||
			vertical != m_VerticalValue)
		{
			m_HorizontalValue = horizontal;
			m_VerticalValue = vertical;

			update();
		}
	}

	void WidgetDualAxis::setRange(float range, float treshold)
	{
		m_Range = range;
		m_Treshold = treshold;

		update();
	}

	void WidgetDualAxis::paintEvent(QPaintEvent* event)
	{
		QRect paint_rect = event->rect();

		int half_width = (paint_rect.width() / 2) - 2;
		int half_height = (paint_rect.height() / 2) - 2;

		float stick_x = m_HorizontalValue / m_Range;
		float stick_y = m_VerticalValue / m_Range;

		QPainter painter(this);

		painter.drawEllipse(
			paint_rect.center(),
			half_width,
			half_height);

		painter.drawLine(
			paint_rect.center(),
			QPoint(
				paint_rect.center().x() + (int)(stick_x * half_width),
				paint_rect.center().y() - (int)(stick_y * half_height)));

		painter.drawText(
			paint_rect,
			QString::number(stick_x),
			QTextOption(Qt::AlignHCenter | Qt::AlignCenter));
	}

}; // namespace Pitstop