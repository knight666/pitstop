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

	void WidgetDualAxis::setTitle(const QString& title)
	{
		m_Title = title;

		update();
	}

	void WidgetDualAxis::paintEvent(QPaintEvent* event)
	{
		int text_height = 20;

		QRect paint_rect = event->rect();

		paint_rect.setHeight(event->rect().height() - (text_height * 2));
		paint_rect.setWidth(paint_rect.height());
		paint_rect.moveLeft((event->rect().width() - paint_rect.width()) / 2);
		paint_rect.moveTop(text_height);

		QRectF title_text_rect(
				event->rect().topLeft(),
				QPointF(
					event->rect().right(),
					text_height));

		QRectF axis_text_rect(
				QPointF(
					event->rect().left(),
					event->rect().height() - text_height),
				QPointF(
					event->rect().right(),
					event->rect().bottom()));

		int half_width = (paint_rect.width() / 2) - 2;
		int half_height = (paint_rect.height() / 2) - 2;

		float stick_x = m_HorizontalValue / m_Range;
		float stick_y = m_VerticalValue / m_Range;

		QPainter painter(this);

		// Outline

		painter.setPen(QPen(QColor(0, 0, 0)));
		painter.drawEllipse(
			paint_rect.center(),
			half_width,
			half_height);

		// Stick

		painter.setPen(QPen(QColor(128, 128, 255)));
		painter.drawLine(
			paint_rect.center(),
			QPoint(
				paint_rect.center().x() + (int)(stick_x * half_width),
				paint_rect.center().y() - (int)(stick_y * half_height)));

		// Treshold

		painter.setPen(QPen(QColor(196, 33, 33)));
		painter.drawEllipse(
			paint_rect.center(),
			(int)((m_Treshold / m_Range) * half_width),
			(int)((m_Treshold / m_Range) * half_height));

		// Text

		painter.setPen(QPen(QColor(0, 0, 0)));
		
		painter.drawText(
			title_text_rect,
			m_Title,
			QTextOption(Qt::AlignHCenter | Qt::AlignCenter));

		painter.drawText(
			axis_text_rect,
			QString("(%1, %2)")
				.arg((int)m_HorizontalValue)
				.arg((int)m_VerticalValue),
			QTextOption(Qt::AlignHCenter | Qt::AlignCenter));
	}

}; // namespace Pitstop