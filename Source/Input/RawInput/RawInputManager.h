#pragma once

#include <Core/Main.h>
#include "Input/Process/InputProcessorBase.h"
#include "Input/RawInput/RawInputJoystick.h"

namespace Pitstop {

	class ContainerManager;
	class RawInputJoystick;

	class RawInputManager
		: public QObject
	{

		Q_OBJECT

	public:

		RawInputManager(QSharedPointer<ContainerManager> containers);
		~RawInputManager();

		bool initialize(HWND window);

		bool updateRegisteredDevices();

		void processInput(LPARAM lParam, WPARAM wParam);
		void processConnectionChanged(LPARAM lParam, WPARAM wParam);

		QSharedPointer<RawInputJoystick> createJoystick(const QString& devicePath);
		QSharedPointer<RawInputJoystick> createJoystick(HANDLE device);
		QSharedPointer<RawInputJoystick> createJoystick(const QJsonObject& serialized);

		// TEMP
		QSharedPointer<RawInputJoystick> getJoystick() const;

		QSharedPointer<RawInputJoystick> getJoystickByIdentifier(const QString& identifier) const;
		QSharedPointer<RawInputJoystick> getJoystickByDevicePath(const QString& devicePath) const;
		QSharedPointer<RawInputJoystick> getJoystickByHandle(HANDLE device) const;

		QVector<QSharedPointer<RawInputJoystick>> getJoysticks() const;

		QSharedPointer<QImage> getJoystickThumbnail(uint16_t vendor, uint16_t product) const;

		InputProcessorBase* createInputProcessor(RawInputJoystick& joystick);

		template <typename InputType>
		void registerInputProcessor()
		{
			registerInputProcessor(
				InputType::Vendor,
				InputType::Product,
				InputType::create);
		}

	signals:

		void signalJoystickCreated(QSharedPointer<RawInputJoystick> joystick);
		void signalJoystickConnected(QSharedPointer<RawInputJoystick> joystick, bool connected);
		void signalJoystickInput(QSharedPointer<RawInputJoystick> joystick);

	private:

		QString getDevicePath(HANDLE device);
		void registerInputProcessor(uint16_t vendor, uint16_t product, InputProcessorBase::FactoryMethod method);
		void addThumbnailImage(uint16_t vendor, uint16_t product, const QString& thumbnailPath);

	private:

		QSharedPointer<ContainerManager> m_Containers;
		bool m_Initialized;
		HWND m_Window;
		QHash<HANDLE, QSharedPointer<RawInputJoystick>> m_JoysticksByHandle;
		QHash<QString, QSharedPointer<RawInputJoystick>> m_JoysticksByIdentifier;
		QHash<uint32_t, QSharedPointer<QImage>> m_JoystickThumbnails;
		QHash<uint32_t, std::function<InputProcessorBase::FactoryMethod>> m_InputProcessorFactories;

	}; // class RawInputManager

}; // namespace Pitstop