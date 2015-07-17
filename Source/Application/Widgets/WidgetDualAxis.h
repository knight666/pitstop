#pragma once

#include "Base/Main.h"

#include <QtWidgets/QWidget>

namespace Pitstop {

	class WidgetDualAxis
		: public QWidget
	{

		Q_OBJECT

	public:

		WidgetDualAxis(QWidget* parent = nullptr);
		~WidgetDualAxis();

		void setAxisValues(float horizontal, float vertical);
		void setPressed(bool value);

		void setRange(float range, float treshold);

		void setTitle(const QString& title);

	private:

		virtual void paintEvent(QPaintEvent* event) override;

	private:

		float m_HorizontalValue;
		float m_VerticalValue;
		bool m_Pressed;
		float m_Range;
		float m_Treshold;
		QString m_Title;

	}; // class WidgetDualAxis

}; // namespace Pitstop