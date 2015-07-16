#include "Application/Widgets/WidgetNeedleIndicator.h"

#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>

namespace Pitstop {

	WidgetNeedleIndicator::WidgetNeedleIndicator(QWidget* parent /*= nullptr*/)
		: QWidget(parent)
		, m_Minimum(0.0f)
		, m_Maximum(1.0f)
		, m_StepSize(0.1f)
		, m_Value(0.0f)
	{
	}

	WidgetNeedleIndicator::~WidgetNeedleIndicator()
	{
	}

	void WidgetNeedleIndicator::setValue(float value)
	{
		value = qMax(qMin(value, m_Maximum), m_Minimum);
		if (value != m_Value)
		{
			m_Value = value;

			update();
		}
	}

	void WidgetNeedleIndicator::setProperties(float minimum, float maximum, float stepSize /*= 0.1f*/)
	{
		m_Minimum = minimum;
		m_Maximum = maximum;
		m_StepSize = stepSize;

		update();
	}

	void WidgetNeedleIndicator::paintEvent(QPaintEvent* event)
	{
		QPoint area_top_left = event->rect().topLeft();
		QPoint area_top_right = event->rect().bottomRight();

		int step_count = (int)((m_Maximum - m_Minimum) / m_StepSize);
		int step_length = (event->rect().right() - event->rect().left()) / step_count;

		int step_center_y = event->rect().center().y();
		int step_half_height = event->rect().height() / 4;

		int value_offset = (int)(((m_Value - m_Minimum) / (m_Maximum - m_Minimum)) * event->rect().width());
		int value_half_height = event->rect().height() / 3;

		QPainter painter(this);

		// Background

		painter.fillRect(event->rect(), QColor(255, 255, 255));

		// Value

		QRect value_rect;
		value_rect.setTopLeft(
			QPoint(
				event->rect().left(),
				event->rect().center().y() - value_half_height));
		value_rect.setBottomRight(
			QPoint(
				event->rect().left() + value_offset,
				event->rect().center().y() + value_half_height));

		painter.fillRect(value_rect, QColor(128, 128, 255));

		// Center

		painter.drawLine(
			QPoint(event->rect().left(), event->rect().center().y()),
			QPoint(event->rect().right(), event->rect().center().y()));

		// Outline

		painter.drawLine(
			event->rect().topLeft(),
			event->rect().bottomLeft());

		painter.drawLine(
			event->rect().topRight(),
			event->rect().bottomRight());

		// Steps

		int step_x = event->rect().left() + step_length;

		for (int i = 1; i < step_count; ++i)
		{
			int half_height = step_half_height;

			if (i == step_count / 2)
			{
				half_height = event->rect().height() / 2;
			}

			painter.drawLine(
				QPoint(step_x, step_center_y - half_height),
				QPoint(step_x, step_center_y + half_height));

			step_x += step_length;
		}
	}

}; // namespace Pitstop