#pragma once

#include "Base/Main.h"

#define SERIALIZATION_VERSION 1

namespace Pitstop
{

	class ISerializable
	{

	public:

		virtual ~ISerializable() {}

		virtual bool Serialize(QJsonObject& target, size_t version = SERIALIZATION_VERSION) = 0;
		virtual bool Deserialize(const QJsonObject& source, size_t version = SERIALIZATION_VERSION) = 0;

	}; // class ISerializable

}; // namespace Pitstop