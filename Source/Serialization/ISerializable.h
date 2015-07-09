#pragma once

#include "Base/Main.h"

#define SERIALIZATION_VERSION 2

namespace Pitstop
{

	class ISerializable
	{

	public:

		virtual ~ISerializable() {}

		virtual bool serialize(QJsonObject& target, size_t version = SERIALIZATION_VERSION) = 0;
		virtual bool deserialize(const QJsonObject& source, size_t version = SERIALIZATION_VERSION) = 0;

	}; // class ISerializable

}; // namespace Pitstop