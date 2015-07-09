#pragma once

#include "Base/Main.h"
#include "Input/XInputState.h"
#include "Serialization/ISerializable.h"

namespace Pitstop {

	class UsbController;

	class UsbDevice
		: public ISerializable
	{

	public:

		UsbDevice(UsbController& controller, uint8_t identifier);
		~UsbDevice();

		uint8_t getIdentifier() const { return m_Identifier; }

		bool isConnected() const { return m_Connected; }
		void setConnected(bool value);

		bool write(DWORD command, QVector<uint8_t>& input, QVector<uint8_t>& output);

		virtual bool serialize(QJsonObject& target, size_t version) override;

	private:

		UsbController& m_Controller;
		uint8_t m_Identifier;
		bool m_Connected;

	}; // class UsbDevice

	typedef QSharedPointer<UsbDevice> UsbDevicePtr;

}; // namespace Pitstop