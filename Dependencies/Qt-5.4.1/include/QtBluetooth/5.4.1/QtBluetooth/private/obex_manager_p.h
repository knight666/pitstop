/*
 * This file was generated by qdbusxml2cpp version 0.7
 * Command line was: qdbusxml2cpp -p obex_manager_p.h:obex_manager.cpp org.openobex.all.xml org.openobex.Manager
 *
 * Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#ifndef OBEX_MANAGER_P_H_1291186314
#define OBEX_MANAGER_P_H_1291186314

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

/*
 * Proxy class for interface org.openobex.Manager
 */
class OrgOpenobexManagerInterface: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "org.openobex.Manager"; }

public:
    OrgOpenobexManagerInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

    ~OrgOpenobexManagerInterface();

public Q_SLOTS: // METHODS
    inline QDBusPendingReply<> RegisterAgent(const QDBusObjectPath &in0)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(in0);
        return asyncCallWithArgumentList(QLatin1String("RegisterAgent"), argumentList);
    }

    inline QDBusPendingReply<> UnregisterAgent(const QDBusObjectPath &in0)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(in0);
        return asyncCallWithArgumentList(QLatin1String("UnregisterAgent"), argumentList);
    }

Q_SIGNALS: // SIGNALS
    void SessionCreated(const QDBusObjectPath &in0);
    void SessionRemoved(const QDBusObjectPath &in0);
    void TransferCompleted(const QDBusObjectPath &in0, bool in1);
    void TransferStarted(const QDBusObjectPath &in0);
};

namespace org {
  namespace openobex {
    typedef ::OrgOpenobexManagerInterface Manager;
  }
}
#endif
