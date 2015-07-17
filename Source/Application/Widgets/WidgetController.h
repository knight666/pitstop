#pragma once

#include "Base/Main.h"

#include <QtWidgets/QWidget>
#include <QtWidgets/QPushButton>

namespace Pitstop {

	class XInputDevice;

	class WidgetController
		: public QPushButton
	{

		Q_OBJECT

	public:

		WidgetController(QWidget* parent = nullptr);
		~WidgetController();

		void setDevice(QSharedPointer<XInputDevice> device);

	private slots:

		void slotDeviceChanged();

	private:

		virtual void paintEvent(QPaintEvent* event) override;

	private:

		QSharedPointer<XInputDevice> m_Device;
		QPixmap m_Icon;
		bool m_IconDirty;

	}; // class WidgetController

}; // namespace Pitstop