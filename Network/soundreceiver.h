#pragma once

#include <QIODevice>
#include <QAudioOutput>

#include "networkobject.h"


class SoundReceiver : public NetworkObject
{
    Q_OBJECT

public:
    explicit SoundReceiver();
    void updateParameters(const QAudioDeviceInfo newDeviceInfo, const QAudioFormat newAudioFormat);
    bool start();
    void stop();
    QHostAddress getHost();
    quint16 getPort();
    bool tryToConnect(const QHostAddress address,const quint16 port);
    void disconnectFromHost();
    bool mute();
    ~SoundReceiver();

public slots:
    void setBufferLimit(int number);

private slots:
    void readDatagrams();
    void handleStateChanged(QAudio::State newState);
    void handleDisconnected();
    void configure();
    void writeDataToDevice();

private:
    void truncateBuffer();
    void readConfiguration();

    QTcpSocket* senderSocket = nullptr;
    QAudioOutput* audioOutput = nullptr;
    QIODevice* audioDevice = nullptr;
    bool configurationWasRead = false;
    bool isGoodConfiguration = false;

    QByteArray buffer;
    int bufferSize = 15392; /// size for writing to device
    int bufferLimit = bufferSize * 5; /// max buffer size
};

