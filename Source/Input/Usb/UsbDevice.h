#pragma once

#include "Base/Main.h"

#include "Input/XInputState.h"

namespace Pitstop {

	class UsbController;

	class UsbDevice
	{

	public:

		UsbDevice(UsbController& controller, uint8_t identifier);
		~UsbDevice();

		bool isPluggedIn() const { return m_PluggedIn; }
		void setPluggedIn(bool value);

		bool write(DWORD command, QVector<uint8_t>& input, QVector<uint8_t>& output);

	private:

		UsbController& m_Controller;
		uint8_t m_Identifier;
		bool m_PluggedIn;

	}; // class UsbDevice

}; // namespace Pitstop