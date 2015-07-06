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
			(const ushort*)&message_buffer[0],
			wcslen(&message_buffer[0]));

		// strip newline at end

		int newLine = message.lastIndexOf("\r\n");
		if (newLine > 0)
		{
			message.remove(newLine - 1, message.length());
		}

		return message;
	}

}; // namespace Pitstop