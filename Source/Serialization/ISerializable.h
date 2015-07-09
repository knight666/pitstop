#pragma once

#include "Base/Main.h"

namespace Pitstop {

	class ISerializable
	{

	public:

		virtual ~ISerializable() {}

		virtual bool serialize(QJsonObject& target, size_t version) = 0;
		virtual bool deserialize(const QJsonObject& source, size_t version) = 0;

	}; // class ISerializable

}; // namespace Pitstop