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

		void setRange(float minimum, float maximum, float stepSize, float treshold);

		void setTitle(const QString& title);

	private:

		virtual void paintEvent(QPaintEvent* event) override;

	private:

		float m_Minimum;
		float m_Maximum;
		float m_StepSize;
		float m_Value;
		float m_Treshold;
		QString m_Title;

	}; // class WidgetSingleAxis

}; // namespace Pitstop