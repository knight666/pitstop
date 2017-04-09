#pragma once

#include <QtCore/qsystemdetection.h>

// Fix compile error in QDateTime

#ifdef Q_OS_WIN
	#define NOMINMAX
#endif

// C/C++

#include <stdint.h>

// STL

#include <functional>

// Windows

#include <Windows.h>

#pragma pack(push, 8)
#include <Dbt.h>
#pragma pack(pop)

// XInput

#include <Xinput.h>

// WinDDK

extern "C" {
	#include <api/hidsdi.h>
	#include <api/SetupAPI.h>
	#include <ddk/hidclass.h>
	#include <hid/hidclient/hid.h>
};

// Qt Core

#include <QtCore/QBuffer>
#include <QtCore/QCommandLineOption>
#include <QtCore/QCommandLineParser>
#include <QtCore/QCoreApplication>
#include <QtCore/QCryptographicHash>
#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QElapsedTimer>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QFileInfoList>
#include <QtCore/QHash>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtCore/QList>
#include <QtCore/QMetaEnum>
#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtCore/QProcess>
#include <QtCore/QQueue>
#include <QtCore/QRegExp>
#include <QtCore/QSet>
#include <QtCore/QSettings>
#include <QtCore/QSharedPointer>
#include <QtCore/QStack>
#include <QtCore/QStandardPaths>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>
#include <QtCore/QThread>
#include <QtCore/QTimer>
#include <QtCore/QUrl>
#include <QtCore/QUrlQuery>
#include <QtCore/QVariant>
#include <QtCore/QWaitCondition>

#include "Base/Utils.h"
#include "Logging/Context.h"
#include "Logging/Logger.h"