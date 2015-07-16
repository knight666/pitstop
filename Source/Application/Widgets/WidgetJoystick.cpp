#include "Application/Widgets/WidgetJoystick.h"

#include "Application/Dialogs/DialogJoystickDetails.h"

namespace Pitstop {

	WidgetJoystick::WidgetJoystick(QSharedPointer<RawInputJoystick> joystick, QWidget* parent /*= nullptr*/)
		: QWidget(parent)
		, m_Joystick(joystick)
	{
		m_Form.setupUi(this);

		connect(
			m_Joystick.data(), SIGNAL(signalPropertyChanged()),
			this, SLOT(slotJoystickPropertyChanged()));

		connect(
			m_Joystick.data(), SIGNAL(signalConnected(RawInputJoystick&, bool)),
			this, SLOT(slotJoystickConnected(RawInputJoystick&, bool)));

		slotJoystickPropertyChanged();
		slotJoystickConnected(*m_Joystick, m_Joystick->isConnected());
	}

	WidgetJoystick::~WidgetJoystick()
	{
		disconnect(
			this, SLOT(slotJoystickConnected(RawInputJoystick&, bool)));

		disconnect(
			this, SLOT(slotJoystickPropertyChanged()));
	}

	void WidgetJoystick::slotJoystickPropertyChanged()
	{
		// Name

		m_Form.lblName->setText(
			QString("%1")
			.arg(m_Joystick->getDescription()));

		// Thumbnail

		QSharedPointer<QImage> thumbnail = m_Joystick->getThumbnail();

		if (thumbnail != nullptr &&
			!thumbnail.isNull())
		{
			m_Form.lblImage->setPixmap(QPixmap::fromImage(*thumbnail));
			m_Form.lblImage->adjustSize();
		}

		// Link

		m_Form.btnLink->setDisabled(m_Joystick->getType() == RawInputJoystick::Type::XInput);

		// XInput

		m_Form.lblXinput->setText(
			QString("%1")
			.arg(m_Joystick->getXinputIndex()));
	}

	void WidgetJoystick::slotJoystickConnected(RawInputJoystick& joystick, bool connected)
	{
		QString icon_resource = ":/Icons/Resources/";

		if (connected)
		{
			icon_resource += "Connected-50.png";
		}
		else
		{
			icon_resource += "Disconnected-50.png";
		}

		m_Form.icoConnected->setPixmap(QPixmap(icon_resource));
	}

	void WidgetJoystick::on_btnDetails_pressed()
	{
		DialogJoystickDetails details(*this);
		details.exec();
	}

}; // namespace Pitstop