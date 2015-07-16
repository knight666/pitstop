#pragma once

#include "Base/Main.h"

#include <QtWidgets/QWidget>

namespace Pitstop {

	class WidgetSingleAxis
		: public QWidget
	{

		Q_OBJECT

	public:

		WidgetSingleAxis(QWidget* parent = nullptr);
		~WidgetSingleAxis();

		void setValue(float value);

		void setProperties(float minimum, float maximum, float stepSize = 0.1f);

	private:

		virtual void paintEvent(QPaintEvent* event) override;

	private:

		float m_Minimum;
		float m_Maximum;
		float m_StepSize;
		float m_Value;

	}; // class WidgetSingleAxis

}; // namespace Pitstop