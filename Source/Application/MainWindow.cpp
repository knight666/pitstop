#include "Application/MainWindow.h"

#include "Input/RawInputManager.h"

namespace Pitstop {

	MainWindow::MainWindow(RawInputManager& rawInput)
		: QMainWindow(nullptr, 0)
		, m_RawInput(rawInput)
		, m_BindingProcessor(nullptr)
	{
		m_Form.setupUi(this);
	}

	MainWindow::~MainWindow()
	{
	}

	void MainWindow::bindJoystick(RawInputJoystick& joystick)
	{
		m_BindingProcessor = joystick.getInputProcessor();
		if (m_BindingProcessor == nullptr)
		{
			return;
		}

		QList<InputProcessorBase::InputBinding> bindings = m_BindingProcessor->getBindings().values();

		qSort(bindings.begin(), bindings.end(),
			[] (const InputProcessorBase::InputBinding& left, const InputProcessorBase::InputBinding& right) {
				return left.index < right.index;
		});

		for (InputProcessorBase::InputBinding& binding : bindings)
		{
			QLabel* label = new QLabel();

			m_Form.labelsContainer->addWidget(label);

			m_BindingLabels[binding.name] = label;
		}

		updateBindings();
	}

	void MainWindow::updateBindings()
	{
		if (m_BindingProcessor == nullptr)
		{
			return;
		}

		const QHash<QString, InputProcessorBase::InputBinding>& bindings = m_BindingProcessor->getBindings();
		for (QHash<QString, InputProcessorBase::InputBinding>::const_iterator it = bindings.begin(); it != bindings.end(); ++it)
		{
			const InputProcessorBase::InputBinding& binding = it.value();

			QHash<QString, QLabel*>::iterator found = m_BindingLabels.find(it.key());
			if (found != m_BindingLabels.end())
			{
				found.value()->setText(QString("%1: %2").arg(it.key()).arg(binding.digitalValue));
			}
		}

		m_Form.labelsContainer->update();
	}

}; // namespace Pitstop