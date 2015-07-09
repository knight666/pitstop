#include "Base/Utils.h"

namespace Pitstop {

	QString windowsErrorToString(DWORD error, HMODULE module /*= NULL*/)
	{
		QVector<wchar_t> message_buffer(1024);

		DWORD flags = FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
		if (module != NULL)
		{
			flags |= FORMAT_MESSAGE_FROM_HMODULE;
		}

		if (::FormatMessageW(
			flags,
			module,
			error,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPWSTR)&message_buffer[0],
			(DWORD)message_buffer.size(),
			NULL) == 0)
		{
			return "";
		}

		QString message = QString::fromUtf16(
			(const ushort*)&message_buffer[0]);

		// strip newline at end

		int newLine = message.lastIndexOf("\r\n");
		if (newLine > 0)
		{
			message.remove(newLine - 1, message.length());
		}

		return message;
	}

	GUID stringToGuid(const QString& guid)
	{
		GUID result = { 0 };
		::CLSIDFromString(guid.utf16(), &result);

		return result;
	}

	QString guidToString(const GUID& guid)
	{
		OLECHAR* guid_string_data;
		::StringFromCLSID(guid, &guid_string_data);
		QString result = QString::fromUtf16(guid_string_data);
		::CoTaskMemFree(guid_string_data);

		return result;
	}

}; // namespace Pitstop