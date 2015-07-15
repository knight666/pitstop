#pragma once

#include "Base/Main.h"

#include <QtWidgets/QWidget>

namespace Pitstop {

	class XInputDevice;

	class WidgetController
		: public QWidget
	{

		Q_OBJECT

	public:

		WidgetController(QWidget* parent = nullptr);
		~WidgetController();

		void setDevice(QSharedPointer<XInputDevice> device);

	private:

		virtual void paintEvent(QPaintEvent* event) override;

	private:

		QSharedPointer<XInputDevice> m_Device;
		QPixmap m_Icon;

	}; // class WidgetController

}; // namespace Pitstop