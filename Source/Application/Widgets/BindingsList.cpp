#include "BindingsList.h"

#include <QtWidgets/QLabel>

#include "Input/RawInputManager.h"
#include "ui_BindingsList.h"

namespace Pitstop {

	BindingsList::BindingsList(QWidget* parent)
		: QWidget(parent)
		, m_Form(new Ui::BindingsList)
		, m_Processor(nullptr)
	{
		m_Form->setupUi(this);
	}

	BindingsList::~BindingsList()
	{
		delete m_Form;
	}

	void BindingsList::bind(InputProcessorBase& processor)
	{
		QList<InputProcessorBase::InputBinding> bindings = processor.getBindings().values();

		qSort(bindings.begin(), bindings.end(),
			[] (const InputProcessorBase::InputBinding& left, const InputProcessorBase::InputBinding& right) {
				return left.index < right.index;
		});

		QVBoxLayout* bindings_layout = qobject_cast<QVBoxLayout*>(layout());

		for (InputProcessorBase::InputBinding& binding : bindings)
		{
			QLabel* label = new QLabel();

			bindings_layout->addWidget(label);

			m_Labels[binding.name] = label;
		}

		m_Processor = &processor;

		update();
	}

	void BindingsList::update()
	{
		if (m_Processor == nullptr)
		{
			return;
		}

		const QHash<QString, InputProcessorBase::InputBinding>& bindings = m_Processor->getBindings();
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