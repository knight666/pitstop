#pragma once

#include "Base/Main.h"

#include <QtWidgets/QWidget>

namespace Pitstop {

	class WidgetDigital
		: public QWidget
	{

		Q_OBJECT

	public:

		WidgetDigital(QWidget* parent = nullptr);
		~WidgetDigital();

		void setValue(bool value);

		void setImage(const QPixmap& image);

		void setTitle(const QString& title);

	private:

		virtual void paintEvent(QPaintEvent* event) override;

	private:

		bool m_Value;
		QPixmap m_Image;
		QString m_Title;

	}; // class WidgetDigital

}; // namespace Pitstop