#include "Application/Widgets/WidgetControllerList.h"

#include <QtGui/QPainter>

namespace Pitstop {

	WidgetControllerList::WidgetControllerList(QWidget* parent /*= nullptr*/)
		: QWidget(parent)
	{
		m_Form.setupUi(this);

		QPixmap modified(*m_Form.lblController1->pixmap());

		QPainter painter(&modified);
		painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
		painter.fillRect(modified.rect(), qRgb(0, 255, 0));
		painter.end();

		m_Form.lblController1->setPixmap(modified);

		for (size_t i = 0; i < 4; ++i)
		{
			setControllerColor(i, qRgb(99, 99, 99));
		}
	}

	WidgetControllerList::~WidgetControllerList()
	{
	}

	void WidgetControllerList::setControllerColor(size_t index, QRgb color)
	{
		if (index >= 4)
		{
			return;
		}

		QLabel* controller_list[4] = {
			m_Form.lblController1,
			m_Form.lblController2,
			m_Form.lblController3,
			m_Form.lblController4
		};

		QPixmap modified(*controller_list[index]->pixmap());

		QPainter painter(&modified);
		painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
		painter.fillRect(modified.rect(), color);
		painter.end();

		controller_list[index]->setPixmap(modified);
	}

}; // namespace Pitstop