#pragma once

#include "Base/Main.h"

namespace Pitstop {

	class ISerializable
	{

	public:

		virtual ~ISerializable() {}

		virtual bool serialize(QJsonObject& target, size_t version) { return true; }
		virtual bool deserialize(const QJsonObject& source, size_t version) { return true; }

	}; // class ISerializable

}; // namespace Pitstop