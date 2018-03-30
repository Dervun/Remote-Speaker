#pragma once

#include <QObject>
#include <QtNetwork>
#include <QAudioDeviceInfo>


class NetworkObject : public QObject
{
    Q_OBJECT

public:
    NetworkObject();
    virtual void updateParameters(const QAudioDeviceInfo newDeviceInfo, const QAudioFormat newAudioFormat);
    virtual bool start() = 0;
    virtual void stop() = 0;
    virtual QHostAddress getHost() = 0;
    virtual quint16 getPort() = 0;
    virtual bool tryToConnect(const QHostAddress address,const quint16 port) = 0;
    virtual void disconnectFromHost(){}
    virtual bool mute(){}
    virtual ~NetworkObject() {}

signals:
    void connected();
    void disconnected();
    void badConfigure();
    void goodConfigure();
    void stopped();
    void bufferSizeChanged(int newLatency);
    void processedUsec(quint64 usec);

public slots:
    virtual void setBufferLimit(int number){Q_UNUSED(number)}

protected slots:
    virtual void handleStateChanged(QAudio::State state) = 0;
    virtual void handleDisconnected(){}
    virtual void writeDataToDevice(){}
    virtual void bytesWritten(qint64 quantity){Q_UNUSED(quantity)}

protected:
    QAudioDeviceInfo deviceInfo;
    QAudioFormat audioFormat = QAudioFormat();

};

