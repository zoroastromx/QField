/***************************************************************************
 bluetoothreceiver.h - BluetoothReceiver

 ---------------------
 begin                : 18.11.2020
 copyright            : (C) 2020 by David Signer
 email                : david (at) opengis.ch
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef BLUETOOTHRECEIVER_H
#define BLUETOOTHRECEIVER_H

#include <QObject>
#include <QtBluetooth/QBluetoothLocalDevice>
#include <QtBluetooth/QBluetoothSocket>
#include "qgsnmeaconnection.h"
#include "gnsspositioninformation.h"

/**
 * The bluetoothreceiver connects to a device and feeds the QgsNmeaConnection over QBluetoothSocket.
 * It receives QgsGpsInformation and converts it to GnssPositionInformation
 */
class BluetoothReceiver : public QObject
{
    Q_OBJECT

    Q_PROPERTY( GnssPositionInformation lastGnssPositionInformation READ lastGnssPositionInformation NOTIFY lastGnssPositionInformationChanged )
    Q_PROPERTY( QBluetoothSocket::SocketState socketState READ socketState NOTIFY socketStateChanged )
    Q_PROPERTY( QString socketStateString READ socketStateString NOTIFY socketStateStringChanged )
    Q_PROPERTY( bool connected READ connected NOTIFY connectedChanged ) //could be removed when QBluetoothSocket can be accessed by the QML

  public:
    explicit BluetoothReceiver( QObject *parent = nullptr );

    Q_INVOKABLE void disconnectDevice();
    Q_INVOKABLE void connectDevice( const QString &address );

    GnssPositionInformation lastGnssPositionInformation() const { return mLastGnssPositionInformation; }
    QBluetoothSocket::SocketState socketState() const { return mSocketState;}
    QString socketStateString() const { return mSocketStateString; }
    bool connected() const  { return mConnected; } //could be removed when QBluetoothSocket can be accessed by the QML

    /**
     * returns a GnssPositionInformation generated by a QGeoPositionInfo
     * from a name defined QGeoPositionInfoSource
     */
    static Q_INVOKABLE GnssPositionInformation fromQGeoPositionInfo( const QString &name );

  signals:
    void lastGnssPositionInformationChanged( GnssPositionInformation lastGnssPositionInformation );
    void socketStateChanged( QBluetoothSocket::SocketState socketState );
    void socketStateStringChanged( QString socketStateString );
    void connectedChanged( bool connected ); //could be removed when QBluetoothSocket can be accessed by the QML

  private slots:
    /**
     * these functions used for repairing are only needed in the linux (not android) environment
     * since there are troubles by connect to a paired device. Maybe it can be resolved in future
     * but meanwhile we keep them as the developer setup.
     */
#ifndef Q_OS_ANDROID
    void pairingFinished( const QBluetoothAddress &address, QBluetoothLocalDevice::Pairing status );
    void confirmPairing( const QBluetoothAddress &address, QString pin );
#endif
    void stateChanged( const QgsGpsInformation &info );
    void setSocketState( const QBluetoothSocket::SocketState socketState );
    void setSocketStateString( const QBluetoothSocket::SocketState socketState );

  private:
#ifndef Q_OS_ANDROID
    void connectService( const QBluetoothAddress &address );
    void repairDevice( const QBluetoothAddress &address );
#endif

    std::unique_ptr<QBluetoothLocalDevice> mLocalDevice;
    std::unique_ptr<QBluetoothSocket> mSocket;
    std::unique_ptr<QgsNmeaConnection> mGpsConnection;
    GnssPositionInformation mLastGnssPositionInformation;
    QBluetoothSocket::SocketState mSocketState;
    QString mSocketStateString;
    bool mConnected = false;
};

#endif // BLUETOOTHRECEIVER_H
