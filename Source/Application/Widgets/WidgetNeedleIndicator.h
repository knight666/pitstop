#pragma once

#include "Base/Main.h"

#include <QtWidgets/QWidget>

namespace Pitstop {

	class WidgetNeedleIndicator
		: public QWidget
	{

		Q_OBJECT

	public:

		WidgetNeedleIndicator(QWidget* parent = nullptr);
		~WidgetNeedleIndicator();

		void setValue(float value);

		void setProperties(float minimum, float maximum, float stepSize = 0.1f);

	private:

		virtual void paintEvent(QPaintEvent* event) override;

	private:

		float m_Minimum;
		float m_Maximum;
		float m_StepSize;
		float m_Value;

	}; // class WidgetNeedleIndicator

}; // namespace Pitstop