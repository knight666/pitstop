#include "Application/Widgets/WidgetDigital.h"

#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>

namespace Pitstop {

	WidgetDigital::WidgetDigital(QWidget* parent /*= nullptr*/)
		: QWidget(parent)
		, m_Value(false)
	{
	}

	WidgetDigital::~WidgetDigital()
	{
	}

	void WidgetDigital::setValue(bool value)
	{
		if (m_Value == value)
		{
			return;
		}

		m_Value = value;

		update();
	}

	void WidgetDigital::setImage(const QPixmap& image)
	{
		m_Image = image;

		update();
	}

	void WidgetDigital::setTitle(const QString& title)
	{
		m_Title = title;

		update();
	}

	void WidgetDigital::paintEvent(QPaintEvent* event)
	{
		int title_text_height = 20;

		QRectF title_text_rect = event->rect();
		title_text_rect.setBottom((float)(event->rect().top() + title_text_height));

		QRect paint_rect = event->rect();
		paint_rect.setTop((int)title_text_rect.bottom());

		QRect image_rect;
		image_rect.setWidth(paint_rect.height());
		image_rect.setHeight(paint_rect.height());
		image_rect.moveLeft((event->rect().width() - paint_rect.height()) / 2);
		image_rect.moveTop(paint_rect.top());

		QPainter painter(this);

		// Activated

		if (m_Value)
		{
			painter.setPen(Qt::NoPen);
			painter.setBrush(QBrush(QColor(128, 128, 255)));

			painter.drawEllipse(
				image_rect.center(),
				(image_rect.width() / 2) - 2,
				(image_rect.height() / 2) - 2);
		}

		// Image

		painter.setPen(Qt::NoPen);
		painter.setBrush(Qt::NoBrush);

		painter.drawPixmap(image_rect, m_Image);

		// Text

		painter.setPen(QPen(QColor(0, 0, 0)));

		painter.drawText(
			title_text_rect,
			m_Title,
			QTextOption(Qt::AlignHCenter | Qt::AlignVCenter));
	}

}; // namespace Pitstop