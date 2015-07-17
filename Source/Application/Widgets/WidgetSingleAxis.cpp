#include "Application/Widgets/WidgetSingleAxis.h"

#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>

namespace Pitstop {

	WidgetSingleAxis::WidgetSingleAxis(QWidget* parent /*= nullptr*/)
		: QWidget(parent)
		, m_Minimum(0.0f)
		, m_Maximum(1.0f)
		, m_StepSize(0.1f)
		, m_Treshold(0.0f)
		, m_Value(0.0f)
	{
	}

	WidgetSingleAxis::~WidgetSingleAxis()
	{
	}

	void WidgetSingleAxis::setValue(float value)
	{
		value = qMax(m_Minimum, qMin(value, m_Maximum));
		if (value != m_Value)
		{
			m_Value = value;

			update();
		}
	}

	void WidgetSingleAxis::setRange(float minimum, float maximum, float stepSize, float treshold)
	{
		m_Minimum = minimum;
		m_Maximum = maximum;
		m_StepSize = stepSize;
		m_Treshold = treshold;

		update();
	}

	void WidgetSingleAxis::setTitle(const QString& title)
	{
		m_Title = title;

		update();
	}

	void WidgetSingleAxis::paintEvent(QPaintEvent* event)
	{
		int title_text_width = 100;
		int value_text_width = 50;

		QRectF title_text_rect(
			QPointF(
				event->rect().left(),
				event->rect().top()),
			QPointF(
				event->rect().left() + title_text_width,
				event->rect().bottom()));

		QRectF value_text_rect(
			QPointF(
				event->rect().right() - value_text_width,
				event->rect().top()),
			QPointF(
				event->rect().right(),
				event->rect().bottom()));

		QRect paint_rect = event->rect();
		paint_rect.setLeft(event->rect().left() + title_text_width);
		paint_rect.setRight(event->rect().right() - value_text_width);

		int step_count = (int)((m_Maximum - m_Minimum) / m_StepSize);
		int step_length = (paint_rect.right() - paint_rect.left()) / step_count;

		int step_center_y = paint_rect.center().y();
		int step_half_height = paint_rect.height() / 4;

		int threshold_offset = (int)(((qMin(m_Value, m_Treshold) - m_Minimum) / (m_Maximum - m_Minimum)) * paint_rect.width());
		int treshold_x = (int)(((m_Treshold - m_Minimum) / (m_Maximum - m_Minimum)) * paint_rect.width());

		int value_offset = (int)(((qMax(m_Value - m_Treshold, m_Minimum) - m_Minimum) / (m_Maximum - m_Minimum)) * paint_rect.width());
		int value_half_height = paint_rect.height() / 3;

		QPainter painter(this);

		// Background

		painter.fillRect(paint_rect, QColor(255, 255, 255));

		// Treshold

		QRect treshold_rect;
		treshold_rect.setTopLeft(
			QPoint(
				paint_rect.left(),
				paint_rect.center().y() - value_half_height));
		treshold_rect.setBottomRight(
			QPoint(
				paint_rect.left() + threshold_offset,
				paint_rect.center().y() + value_half_height));

		painter.fillRect(treshold_rect, QColor(196, 33, 33));

		// Value

		QRect value_rect;
		value_rect.setTopLeft(
			QPoint(
				paint_rect.left() + threshold_offset,
				paint_rect.center().y() - value_half_height));
		value_rect.setBottomRight(
			QPoint(
				paint_rect.left() + threshold_offset + value_offset,
				paint_rect.center().y() + value_half_height));

		painter.fillRect(value_rect, QColor(128, 128, 255));

		// Center

		painter.setPen(QPen(QColor(0, 0, 0)));

		painter.drawLine(
			QPoint(paint_rect.left(), paint_rect.center().y()),
			QPoint(paint_rect.right(), paint_rect.center().y()));

		// Outline

		painter.setPen(QPen(QColor(0, 0, 0)));

		painter.drawLine(
			paint_rect.topLeft(),
			paint_rect.bottomLeft());

		painter.drawLine(
			paint_rect.topRight(),
			paint_rect.bottomRight());

		// Steps

		painter.setPen(QPen(QColor(0, 0, 0)));

		int step_x = paint_rect.left() + step_length;

		for (int i = 1; i < step_count; ++i)
		{
			int half_height = step_half_height;

			painter.drawLine(
				QPoint(step_x, step_center_y - half_height),
				QPoint(step_x, step_center_y + half_height));

			step_x += step_length;
		}

		// Treshold line

		painter.setPen(QPen(QColor(0, 0, 0)));
		painter.drawLine(
			QPoint(paint_rect.left() + treshold_x, paint_rect.top()),
			QPoint(paint_rect.left() + treshold_x, paint_rect.bottom()));

		// Text

		painter.setPen(QPen(QColor(0, 0, 0)));

		painter.drawText(
			title_text_rect,
			m_Title,
			QTextOption(Qt::AlignVCenter));

		painter.drawText(
			value_text_rect,
			QString::number((int)m_Value),
			QTextOption(Qt::AlignHCenter | Qt::AlignVCenter));
	}

}; // namespace Pitstop