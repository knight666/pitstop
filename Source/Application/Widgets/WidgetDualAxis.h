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

		void setValue(float horizontal, float vertical);

		void setRange(float range, float treshold);

	private:

		virtual void paintEvent(QPaintEvent* event) override;

	private:

		float m_HorizontalValue;
		float m_VerticalValue;
		float m_Range;
		float m_Treshold;

	}; // class WidgetDualAxis

}; // namespace Pitstop