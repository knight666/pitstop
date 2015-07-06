#include "Application/TabDevices.h"

#include "Input/RawInputManager.h"

namespace Pitstop {

	TabDevices::TabDevices(QWidget* parent /*= nullptr*/, Qt::WindowFlags flags /*= 0*/)
		: QWidget(parent, flags)
	{
		m_Form.setupUi(this);

		m_Form.scrlDevicesContents->setLayout(new QVBoxLayout(m_Form.scrlDevices));
	}

	TabDevices::~TabDevices()
	{
	}

	void TabDevices::setup(RawInputManager& rawInput)
	{
	}

}; // namespace Pitstop