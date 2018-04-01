#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QAudioDeviceInfo>
#include <QIODevice>
#include <QAudioOutput>

class SoundReceiver : public QObject
{
    Q_OBJECT

public:
    explicit SoundReceiver();
    void updateInfo(const QAudioDeviceInfo newDeviceInfo);
    bool tryToConnect(const QHostAddress address,const quint16 port);
    void disconnectFromHost();
    bool mute();
    ~SoundReceiver();

signals:
    void connected();
    void disconnected();
    void badConfigure();
    void goodConfigure();
    void stopped();
    void bufferSizeChanged(int newLatency);
    void processedUsec(quint64 usec);

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

    QAudioDeviceInfo deviceInfo;
    QAudioFormat audioFormat = QAudioFormat();
    QTcpSocket* senderSocket = nullptr;
    QAudioOutput* audioOutput = nullptr;
    QIODevice* audioDevice = nullptr;
    bool configurationWasRead = false;
    bool isGoodConfiguration = false;

    QByteArray buffer;
    int bufferSize = 15392; /// size for writing to device
    int bufferLimit = bufferSize * 5; /// max buffer size
};

