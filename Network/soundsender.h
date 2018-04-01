#pragma once

#include <QObject>
#include <QtNetwork>
#include <QAudioDeviceInfo>
#include <QTcpSocket>
#include <QTcpServer>
#include <QIODevice>
#include <QAudioInput>

class SoundSender : public QObject
{
    Q_OBJECT

public:
    explicit SoundSender();
    void updateInfo(const QAudioDeviceInfo newDeviceInfo, const QAudioFormat newAudioFormat);
    bool start();
    void stop();
    QHostAddress getHost();
    quint16 getPort();
    ~SoundSender();

signals:
    void connected();
    void disconnected();

private slots:
    /*!
     * \brief Been called when client connected to server.
     */
    void newConnection();

    void handleStateChanged(QAudio::State newState);
    void handleDisconnected();
    void bytesWritten(qint64 quantity);

private:
    /*!
     * \return Returns false if server doesn't can to listen this address, otherwise listening starts and return true.
     */
    bool tryToListen(const QHostAddress &address);
    /*!
     * \brief getCurrentSettings
     * \return QString, split by ";"
     */
    QByteArray getCurrentSettings();

    QAudioDeviceInfo deviceInfo;
    QAudioFormat audioFormat = QAudioFormat();
    QTcpServer* server = nullptr;
    QTcpSocket* receiverSocket = nullptr;

    QAudioInput* audioInput = nullptr;
    QIODevice* audioDevice = nullptr;
};
