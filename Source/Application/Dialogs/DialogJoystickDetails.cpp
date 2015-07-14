#include "Application/Dialogs/DialogJoystickDetails.h"

#include "Application/Widgets/WidgetJoystick.h"

namespace Pitstop {

	DialogJoystickDetails::DialogJoystickDetails(WidgetJoystick& widget)
		: QDialog(&widget, 0)
		, m_Joystick(widget.getJoystick())
	{
		m_Form.setupUi(this);

		setWindowTitle(m_Joystick->getDescription());

		QString connected_string;
		QTextStream connected_stream(&connected_string);
		connected_stream << m_Joystick->isConnected();
		addRow("Connected", connected_string);

		QString type_string;
		QTextStream type_stream(&type_string);
		type_stream << m_Joystick->getType();
		addRow("Type", type_string);

		addRow("XInput index", QString::number(m_Joystick->getXinputIndex()));

		addRow("Virtual index", QString::number(m_Joystick->getVirtualIndex()));

		addRow("Enumerator name", m_Joystick->getRegistryProperty<QString>(SPDRP_ENUMERATOR_NAME));

		QString hardware_identifiers_string;
		QStringList hardware_identifiers = m_Joystick->getRegistryProperty<QStringList>(SPDRP_HARDWAREID);
		for (int i = 0; i < hardware_identifiers.length(); ++i)
		{
			if (i > 0)
			{
				hardware_identifiers_string += "; ";
			}
			hardware_identifiers_string += hardware_identifiers[i];
		}

		addRow("Hardware identifiers", hardware_identifiers_string);

		QString vendor_string;
		vendor_string.sprintf("0x%04X", m_Joystick->getInfo().dwVendorId);
		addRow("Vendor identifier", vendor_string);

		QString product_string;
		product_string.sprintf("0x%04X", m_Joystick->getInfo().dwProductId);
		addRow("Product identifier", product_string);

		QString version_string;
		version_string.sprintf("%d", m_Joystick->getInfo().dwVersionNumber);
		addRow("Version number", version_string);

		QString usage_page_string;
		usage_page_string.sprintf("%d", m_Joystick->getInfo().usUsagePage);
		addRow("Usage page", usage_page_string);

		QString usage_string;
		usage_string.sprintf("%d", m_Joystick->getInfo().usUsage);
		addRow("Usage page", usage_string);

		addRow("Class", m_Joystick->getRegistryProperty<QString>(SPDRP_CLASS));

		addRow("Class GUID", m_Joystick->getRegistryProperty<QString>(SPDRP_CLASSGUID));

		addRow("Driver", m_Joystick->getRegistryProperty<QString>(SPDRP_DRIVER));

		addRow("USB class", m_Joystick->getRegistryProperty<QString>(SPDRP_CLASS, RawInputJoystick::DeviceClass::USB));

		addRow("USB location information", m_Joystick->getRegistryProperty<QString>(SPDRP_LOCATION_INFORMATION, RawInputJoystick::DeviceClass::USB));

		addRow("Container identifier", m_Joystick->getRegistryProperty<QString>(SPDRP_BASE_CONTAINERID));

		QString handle_string;
		handle_string.sprintf("0x%08X", m_Joystick->getHandle());
		addRow("Handle", handle_string);

		addRow("GUID", guidToString(m_Joystick->getGuid()));

		addRow("Path", m_Joystick->getDevicePath());
	}

	DialogJoystickDetails::~DialogJoystickDetails()
	{
	}

	void DialogJoystickDetails::addRow(const QString& name, const QString& value)
	{
		int row = m_Form.tblProperties->rowCount();

		m_Form.tblProperties->insertRow(row);

		QTableWidgetItem* item_name = new QTableWidgetItem(name);
		m_Form.tblProperties->setItem(row, 0, item_name);

		QTableWidgetItem* item_value = new QTableWidgetItem(value);
		m_Form.tblProperties->setItem(row, 1, item_value);
	}

}; // namespace Pitstop