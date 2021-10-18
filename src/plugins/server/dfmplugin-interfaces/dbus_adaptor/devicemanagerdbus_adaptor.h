/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -i ../../plugins/server/dfmplugin-interfaces/devicemanagerdbus.h -c DeviceManagerAdaptor -l DeviceManagerDBus -a ../../plugins/server/dfmplugin-interfaces/dbus_adaptor/devicemanagerdbus_adaptor devicemanagerdbus.xml
 *
 * qdbusxml2cpp is Copyright (C) 2017 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#ifndef DEVICEMANAGERDBUS_ADAPTOR_H
#define DEVICEMANAGERDBUS_ADAPTOR_H

#include <QtCore/QObject>
#include <QtDBus/QtDBus>
#include "../../plugins/server/dfmplugin-interfaces/devicemanagerdbus.h"
QT_BEGIN_NAMESPACE
class QByteArray;
template<class T> class QList;
template<class Key, class Value> class QMap;
class QString;
class QStringList;
class QVariant;
QT_END_NAMESPACE

/*
 * Adaptor class for interface com.deepin.filemanager.service.DeviceManager
 */
class DeviceManagerAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.deepin.filemanager.service.DeviceManager")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"com.deepin.filemanager.service.DeviceManager\">\n"
"    <signal name=\"AutoMountCompleted\"/>\n"
"    <method name=\"UnmountAllDevices\"/>\n"
"  </interface>\n"
        "")
public:
    DeviceManagerAdaptor(DeviceManagerDBus *parent);
    virtual ~DeviceManagerAdaptor();

    inline DeviceManagerDBus *parent() const
    { return static_cast<DeviceManagerDBus *>(QObject::parent()); }

public: // PROPERTIES
public Q_SLOTS: // METHODS
    void UnmountAllDevices();
Q_SIGNALS: // SIGNALS
    void AutoMountCompleted();
};

#endif
