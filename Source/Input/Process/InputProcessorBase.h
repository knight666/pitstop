#pragma once

#include <Core/Main.h>
#include "Input/RawInput/RawInputJoystick.h"

namespace Pitstop {

#define IMPLEMENT_INPUT_PROCESSOR(_type, _vid, _pid) \
	static const uint16_t Vendor = _vid; \
	static const uint16_t Product = _pid; \
	static InputProcessorBase* create(RawInputJoystick& joystick) { return new _type(joystick); } \
	_type(RawInputJoystick& joystick) : InputProcessorBase(joystick) { }

	class InputProcessorBase
	{

	public:

		enum InputState
		{
			InputState_Up = 0,
			InputState_Pressed = 0x01,
			InputState_Down = 0x02,
			InputState_Released = 0x04,
		};

		enum class InputType
		{
			Digital,
			Analog
		};

		struct InputBinding
		{
			InputBinding(const QString& name = "", size_t index = 0, InputType type = InputType::Analog)
				: name(name)
				, index(index)
				, type(type)
				, digitalValue(false)
				, analogValue(0.0f)
			{
			}

			QString name;
			size_t index;
			InputType type;
			bool digitalValue;
			float analogValue;
		};

		typedef InputProcessorBase* (FactoryMethod(RawInputJoystick&));

		InputProcessorBase(RawInputJoystick& joystick);
		virtual ~InputProcessorBase();

		const QHash<QString, InputBinding>& getBindings() const { return m_Bindings; }

		virtual bool setup();

		bool process(const RAWINPUT& message);

	protected:

		virtual void createBindings() = 0;

		virtual bool processDigital(USAGE identifier, bool pressed) = 0;
		virtual bool processAnalog(USAGE identifier, LONG value) = 0;

		void addBinding(const QString& name, InputType type);

	protected:

		RawInputJoystick& m_Joystick;
		QHash<QString, InputBinding> m_Bindings;

		QVector<uint8_t> m_PreparsedData;
		PHIDP_PREPARSED_DATA m_Preparsed;
		HIDP_CAPS m_Capabilities;
		QVector<HIDP_BUTTON_CAPS> m_ButtonCapabilities;
		QVector<HIDP_VALUE_CAPS> m_ValueCapabilities;
		QHash<USAGE, bool> m_ButtonsState;
		QHash<USAGE, LONG> m_AxisValues;

	}; // class ProcessorBase

}; // namespace Pitstop