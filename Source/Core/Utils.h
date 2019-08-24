#pragma once

#include <Core/Main.h>

namespace Pitstop {

	QString windowsErrorToString(DWORD error, HMODULE module = NULL);

	GUID stringToGuid(const QString& guid);
	QString guidToString(const GUID& guid);

	bool deviceGetRegistryProperty(HDEVINFO& deviceInfo, SP_DEVINFO_DATA& deviceInfoData, DWORD key, QVector<BYTE>& output, DWORD& keyType);

	template <typename ValueType>
	ValueType deviceGetRegistryProperty(HDEVINFO& deviceInfo, SP_DEVINFO_DATA& deviceInfoData, DWORD key);

	template <>
	inline DWORD deviceGetRegistryProperty(HDEVINFO& deviceInfo, SP_DEVINFO_DATA& deviceInfoData, DWORD key)
	{
		QVector<BYTE> output;
		DWORD key_type = 0;
		bool found = false;

		if (!deviceGetRegistryProperty(deviceInfo, deviceInfoData, key, output, key_type) ||
			key_type != REG_DWORD)
		{
			return 0;
		}

		return *(DWORD*)&output[0];
	}

	template <>
	inline QString deviceGetRegistryProperty(HDEVINFO& deviceInfo, SP_DEVINFO_DATA& deviceInfoData, DWORD key)
	{
		QVector<BYTE> output;
		DWORD key_type = 0;

		if (!deviceGetRegistryProperty(deviceInfo, deviceInfoData, key, output, key_type) ||
			key_type != REG_SZ)
		{
			return QString();
		}

		return QString::fromUtf16((const ushort*)&output[0]);
	}

	template <>
	inline QStringList deviceGetRegistryProperty(HDEVINFO& deviceInfo, SP_DEVINFO_DATA& deviceInfoData, DWORD key)
	{
		QVector<BYTE> output;
		DWORD key_type = 0;

		if (!deviceGetRegistryProperty(deviceInfo, deviceInfoData, key, output, key_type) ||
			key_type != REG_MULTI_SZ)
		{
			return QStringList();
		}

		QStringList result;

		const BYTE* src = &output[0];
		size_t src_length = static_cast<size_t>(output.length()) - sizeof(wchar_t);

		while (src_length > 0)
		{
			const wchar_t* entry_text = (const wchar_t*)src;
			size_t entry_text_length = wcslen(entry_text);
			size_t entry_text_offset = (entry_text_length + 1) * sizeof(wchar_t);

			QString entry = QString::fromUtf16(entry_text, static_cast<int>(entry_text_length));
			result.push_back(entry);

			if (entry_text_offset >= src_length)
			{
				break;
			}

			src += entry_text_offset;
			src_length -= entry_text_offset;
		}

		return result;
	}

}; // namespace Pitstop