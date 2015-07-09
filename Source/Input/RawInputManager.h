#pragma once

#include "Base/Main.h"
#include "Input/Process/InputProcessorBase.h"

namespace Pitstop {

	class RawInputJoystick;

	class RawInputManager
		: public QObject
	{

		Q_OBJECT

	public:

		RawInputManager();
		~RawInputManager();

		bool initialize(HWND window);

		void processInput(LPARAM lParam, WPARAM wParam);
		void processConnectionChanged(LPARAM lParam, WPARAM wParam);

		RawInputJoystickPtr createJoystick(const QString& devicePath);
		RawInputJoystickPtr createJoystick(HANDLE device);
		RawInputJoystickPtr createJoystick(const QJsonObject& serialized);

		// TEMP
		RawInputJoystickPtr getJoystick() const;

		RawInputJoystickPtr getJoystickByPath(const QString& devicePath) const;
		RawInputJoystickPtr getJoystickByHandle(HANDLE device) const;

		QVector<RawInputJoystickPtr> getJoysticks() const;

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

		void signalJoystickCreated(RawInputJoystickPtr joystick);
		void signalJoystickConnected(RawInputJoystickPtr joystick, bool connected);
		void signalJoystickInput(RawInputJoystickPtr joystick);

	private:

		QString getDevicePath(HANDLE device);
		void registerInputProcessor(uint16_t vendor, uint16_t product, InputProcessorBase::FactoryMethod method);
		void addThumbnailImage(uint16_t vendor, uint16_t product, const QString& thumbnailPath);

	private:

		bool m_Initialized;
		HWND m_Window;
		QHash<HANDLE, RawInputJoystickPtr> m_JoysticksByHandle;
		QHash<QString, RawInputJoystickPtr> m_JoysticksByPath;
		QHash<uint32_t, QSharedPointer<QImage>> m_JoystickThumbnails;
		QHash<uint32_t, std::function<InputProcessorBase::FactoryMethod>> m_InputProcessorFactories;

	}; // class RawInputManager

}; // namespace Pitstop