#include "BindingsList.h"

#include <QtWidgets/QLabel>

#include "Input/Process/InputProcessorBase.h"
#include "ui_BindingsList.h"

namespace Pitstop {

	BindingsList::BindingsList(QWidget* parent)
		: QWidget(parent)
		, m_Form(new Ui::BindingsList)
		, m_Joystick(nullptr)
	{
		m_Form->setupUi(this);
	}

	BindingsList::~BindingsList()
	{
		disconnect(
			this, SLOT(slotJoystickInput(RawInputJoystick*, bool)));

		delete m_Form;
	}

	void BindingsList::bind(RawInputJoystick& joystick)
	{
		QVBoxLayout* bindings_layout = qobject_cast<QVBoxLayout*>(layout());

		qDeleteAll(m_Labels);

		connect(
			&joystick, SIGNAL(signalJoystickInput(RawInputJoystick*, bool)),
			this, SLOT(slotJoystickInput(RawInputJoystick*, bool)));

		m_Joystick = &joystick;

		InputProcessorBase* processor = m_Joystick->getInputProcessor();
		if (processor == nullptr)
		{
			return;
		}

		QList<InputProcessorBase::InputBinding> bindings = processor->getBindings().values();

		qSort(bindings.begin(), bindings.end(),
			[] (const InputProcessorBase::InputBinding& left, const InputProcessorBase::InputBinding& right) {
				return left.index < right.index;
		});

		for (InputProcessorBase::InputBinding& binding : bindings)
		{
			QLabel* label = new QLabel();

			bindings_layout->addWidget(label);

			m_Labels[binding.name] = label;
		}

		update();
	}

	void BindingsList::update()
	{
		if (m_Joystick == nullptr ||
			m_Joystick->getInputProcessor() == nullptr)
		{
			return;
		}

		const QHash<QString, InputProcessorBase::InputBinding>& bindings = m_Joystick->getInputProcessor()->getBindings();
		for (QHash<QString, InputProcessorBase::InputBinding>::const_iterator it = bindings.begin(); it != bindings.end(); ++it)
		{
			const InputProcessorBase::InputBinding& binding = it.value();

			QHash<QString, QLabel*>::iterator found = m_Labels.find(it.key());
			if (found != m_Labels.end())
			{
				QString text = QString("%1: %2").arg(it.key());
				if (binding.type == InputProcessorBase::InputType::Digital)
				{
					text = text.arg(binding.digitalValue);
				}
				else
				{
					text = text.arg(binding.analogValue);
				}
				found.value()->setText(text);
			}
		}

		layout()->update();
	}

	void BindingsList::slotJoystickInput(RawInputJoystick* joystick, bool processed)
	{
		if (m_Joystick != joystick ||
			joystick == nullptr)
		{
			return;
		}

		InputProcessorBase* processor = m_Joystick->getInputProcessor();
		if (processor == nullptr)
		{
			return;
		}

		const QHash<QString, InputProcessorBase::InputBinding>& bindings = processor->getBindings();
		for (QHash<QString, InputProcessorBase::InputBinding>::const_iterator it = bindings.begin(); it != bindings.end(); ++it)
		{
			const InputProcessorBase::InputBinding& binding = it.value();

			QHash<QString, QLabel*>::iterator found = m_Labels.find(it.key());
			if (found != m_Labels.end())
			{
				QString text = QString("%1: %2").arg(it.key());
				if (binding.type == InputProcessorBase::InputType::Digital)
				{
					text = text.arg(binding.digitalValue);
				}
				else
				{
					text = text.arg(binding.analogValue);
				}
				found.value()->setText(text);
			}
		}

		layout()->update();
	}

}; // namespace Pitstop