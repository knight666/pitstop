#pragma once

#include "Base/Main.h"

#include "Input/VirtualInputDevice.h"
#include "Serialization/ISerializable.h"

namespace Pitstop {

	class RawInputManager;

	class VirtualInputManager
		: public QObject
		, public ISerializable
	{

		Q_OBJECT

	public:

		VirtualInputManager(RawInputManager& rawInput);
		~VirtualInputManager();

		VirtualInputDevicePtr createDevice();

		VirtualInputDevicePtr getDeviceByIndex(uint8_t index) const;

		virtual bool serialize(QJsonObject& target, size_t version = SERIALIZATION_VERSION) override;
		virtual bool deserialize(const QJsonObject& source, size_t version = SERIALIZATION_VERSION) override;

	private:

		RawInputManager& m_RawInput;
		QVector<VirtualInputDevicePtr> m_Devices;

	}; // class VirtualInputManager

}; // namespace Pitstop