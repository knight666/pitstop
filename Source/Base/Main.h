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

// SetupAPI

#include <SetupAPI.h>

// Qt Core

#include <QtCore/QBuffer>
#include <QtCore/QCommandLineOption>
#include <QtCore/QCommandLineParser>
#include <QtCore/QCoreApplication>
#include <QtCore/QCryptographicHash>
#include <QtCore/QDateTime>
#include <QtCore/QDir>
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
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>
#include <QtCore/QThread>
#include <QtCore/QTimer>
#include <QtCore/QUrl>
#include <QtCore/QUrlQuery>
#include <QtCore/QVariant>
#include <QtCore/QWaitCondition>

extern "C" {
	#include <hidsdi.h>
};

static const QDir g_ApplicationDir = QDir::homePath() + "/Pitstop";