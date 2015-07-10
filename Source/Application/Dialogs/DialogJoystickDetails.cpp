#include "Application/Dialogs/DialogJoystickDetails.h"

namespace Pitstop {

	DialogJoystickDetails::DialogJoystickDetails(QWidget* parent /*= nullptr*/, Qt::WindowFlags flags /*= 0*/)
		: QDialog(parent, flags)
	{
		m_Form.setupUi(this);
	}

	DialogJoystickDetails::~DialogJoystickDetails()
	{

	}

}; // namespace Pitstop