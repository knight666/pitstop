#include "InspectorApplication.h"

#include <Input/Container/ContainerManager.h>
#include <Input/RawInput/RawInputManager.h>

namespace Pitstop {

	InspectorApplication::InspectorApplication(int& argc, char** argv, int flags /*= ApplicationFlags*/)
		: QApplication(argc, argv, flags)
		, m_Containers(new ContainerManager())
		, m_RawInput(new RawInputManager(m_Containers))
		, m_Driver(new InspectorDriver(m_Tracking))
	{
		setApplicationName("PitstopInspector");

		m_MainWindowForm.setupUi(&m_MainWindow);

		m_MainWindowForm.cmbJoysticks->clear();
		m_MainWindowForm.cmbJoysticks->addItem("<None>");

		m_MainWindowForm.btnStop->setEnabled(false);

		connect(
			m_MainWindowForm.btnStart, SIGNAL(pressed()),
			this, SLOT(on_btnStart_pressed()));

		connect(
			m_MainWindowForm.btnStop, SIGNAL(pressed()),
			this, SLOT(on_btnStop_pressed()));

		connect(
			m_RawInput.data(), SIGNAL(signalJoystickConnected(QSharedPointer<RawInputJoystick>, bool)),
			this, SLOT(slotJoystickConnected(QSharedPointer<RawInputJoystick>, bool)));

		connect(
			&m_Tracking, SIGNAL(signalTrackingUpdated(USAGE, TrackingItem&)),
			this, SLOT(slotTrackingUpdated(USAGE, TrackingItem&)));

		installNativeEventFilter(this);
	}

	InspectorApplication::~InspectorApplication()
	{
		disconnect(
			this, SLOT(slotJoystickConnected(QSharedPointer<RawInputJoystick>, bool)));
	}

	int InspectorApplication::run()
	{
		if (!m_Containers->initialize())
		{
			PS_LOG_ERROR(ContainerManager) << "Failed to initialize containers.";

			return -1;
		}

		if (!m_RawInput->initialize((HWND)m_MainWindow.winId()))
		{
			PS_LOG_ERROR(RawInputManager) << "Failed to initialize raw input.";

			return -1;
		}

		m_MainWindow.show();

		return exec();
	}

	void InspectorApplication::slotJoystickConnected(QSharedPointer<RawInputJoystick> joystick, bool connected)
	{
		int selected = 0;
		int current = 1;

		bool previous = m_MainWindowForm.cmbJoysticks->blockSignals(true);

		m_MainWindowForm.cmbJoysticks->clear();
		m_MainWindowForm.cmbJoysticks->addItem("<None>");

		QVector<QSharedPointer<RawInputJoystick>> joysticks = m_RawInput->getJoysticks();
		for (QSharedPointer<RawInputJoystick>& joystick : joysticks)
		{
			if (joystick->getType() != RawInputJoystick::Type::RawInput)
			{
				continue;
			}

			if (m_JoystickSelected != nullptr &&
				joystick == m_JoystickSelected)
			{
				selected = current;
			}

			m_MainWindowForm.cmbJoysticks->addItem(
				joystick->getDescription(),
				QVariant(joystick->getIdentifier()));

			current++;
		}

		m_MainWindowForm.cmbJoysticks->blockSignals(previous);

		m_MainWindowForm.cmbJoysticks->setCurrentIndex(selected);
	}

	void InspectorApplication::slotTrackingUpdated(USAGE identifier, TrackingItem& item)
	{
		int row_index = -1;
		QTableWidgetItem* item_latest = nullptr;
		QTableWidgetItem* item_minimum = nullptr;
		QTableWidgetItem* item_maximum = nullptr;
		QTableWidgetItem* item_average = nullptr;

		auto found = m_TrackingRows.find(identifier);
		if (found != m_TrackingRows.end())
		{
			row_index = found.value();

			item_latest = m_MainWindowForm.tblValues->item(row_index, 2);
			item_minimum = m_MainWindowForm.tblValues->item(row_index, 3);
			item_maximum = m_MainWindowForm.tblValues->item(row_index, 4);
			item_average = m_MainWindowForm.tblValues->item(row_index, 5);
		}
		else
		{
			row_index = m_MainWindowForm.tblValues->rowCount();
			m_MainWindowForm.tblValues->insertRow(row_index);

			QTableWidgetItem* item_id = new QTableWidgetItem(QString("%1").arg(identifier));
			m_MainWindowForm.tblValues->setItem(row_index, 0, item_id);

			QTableWidgetItem* item_name = new QTableWidgetItem(item.getName());
			m_MainWindowForm.tblValues->setItem(row_index, 1, item_name);

			item_latest = new QTableWidgetItem();
			m_MainWindowForm.tblValues->setItem(row_index, 2, item_latest);

			item_minimum = new QTableWidgetItem();
			m_MainWindowForm.tblValues->setItem(row_index, 3, item_minimum);

			item_maximum = new QTableWidgetItem();
			m_MainWindowForm.tblValues->setItem(row_index, 4, item_maximum);

			item_average = new QTableWidgetItem();
			m_MainWindowForm.tblValues->setItem(row_index, 5, item_average);

			m_TrackingRows[identifier] = row_index;
		}

		item_latest->setText(QString("%1").arg(item.getValueLatest()));
		item_minimum->setText(QString("%1").arg(item.getMinimum()));
		item_maximum->setText(QString("%1").arg(item.getMaximum()));
		item_average->setText(QString("%1").arg(item.getAverage()));
	}

	void InspectorApplication::on_btnStart_pressed()
	{
		int selected = m_MainWindowForm.cmbJoysticks->currentIndex();
		if (selected == 0)
		{
			return;
		}

		QString joystick_path = m_MainWindowForm.cmbJoysticks->currentData().toString();
		auto joystick = m_RawInput->getJoystickByIdentifier(joystick_path);
		if (joystick == nullptr)
		{
			return;
		}

		m_Tracking.clear();

		m_JoystickSelected = joystick;
		m_JoystickSelected->setInputProcessor(m_Driver.data());

		m_MainWindowForm.btnOpen->setEnabled(false);
		m_MainWindowForm.btnSave->setEnabled(false);
		m_MainWindowForm.cmbJoysticks->setEnabled(false);
		m_MainWindowForm.btnStart->setEnabled(false);
		m_MainWindowForm.btnStop->setEnabled(true);
	}

	void InspectorApplication::on_btnStop_pressed()
	{
		if (m_JoystickSelected != nullptr)
		{
			m_JoystickSelected->setInputProcessor(nullptr);
		}

		QJsonDocument document;
		m_Tracking.serialize(document);

		QFile output("joystick.json");
		if (output.open(QIODevice::WriteOnly))
		{
			output.write(document.toJson());
			output.close();
		}

		m_MainWindowForm.btnOpen->setEnabled(true);
		m_MainWindowForm.btnSave->setEnabled(true);
		m_MainWindowForm.cmbJoysticks->setEnabled(true);
		m_MainWindowForm.btnStart->setEnabled(true);
		m_MainWindowForm.btnStop->setEnabled(false);
	}

	bool InspectorApplication::nativeEventFilter(const QByteArray& eventType, void* message, long* result)
	{
		if (eventType != "windows_generic_MSG")
		{
			return false;
		}

		MSG* msg = static_cast<MSG*>(message);

		switch (msg->message)
		{

		case WM_INPUT:
			m_RawInput->processInput(msg->lParam, msg->wParam);
			return true;

		case WM_INPUT_DEVICE_CHANGE:
			m_RawInput->processConnectionChanged(msg->lParam, msg->wParam);
			return true;

		default:
			return false;

		}
	}

}; // namespace Pitstop