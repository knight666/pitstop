#pragma once

#include <Core/Main.h>
#include "Serialization/ConfigurationEventDispatcher.h"
#include "Serialization/ISerializable.h"

namespace Pitstop {

	class UsbController;
	struct XInputState;

	class UsbDevice
		: public ConfigurationEventDispatcher
		, public ISerializable
	{

		Q_OBJECT

	public:

		UsbDevice(
			QSharedPointer<ConfigurationManager> configuration,
			UsbController& controller,
			uint8_t identifier);
		~UsbDevice();

		uint8_t getIdentifier() const { return m_Identifier; }

		bool isConnected() const { return m_Connected; }
		void setConnected(bool value);

		bool write(DWORD command, QVector<uint8_t>& input, QVector<uint8_t>& output);
		bool writeXinputState(const XInputState& state);

		virtual bool serialize(QJsonObject& target, size_t version) override;

	signals:

		void signalConnectionChanged(bool connected);

	private:

		UsbController& m_Controller;
		uint8_t m_Identifier;
		bool m_Connected;

	}; // class UsbDevice

}; // namespace Pitstop