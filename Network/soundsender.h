#pragma once

#include <QTcpSocket>
#include <QTcpServer>
#include <QIODevice>
#include <QAudioInput>

#include "networkobject.h"


class SoundSender : public NetworkObject
{
    Q_OBJECT

public:
    explicit SoundSender();
    bool start();
    void stop();
    QHostAddress getHost();
    quint16 getPort();
     bool tryToConnect(const QHostAddress address,const quint16 port);
    ~SoundSender();

signals:
    void newClient();

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

    QTcpServer* server = nullptr;
    QTcpSocket* receiverSocket = nullptr;

    QAudioInput* audioInput = nullptr;
    QIODevice* audioDevice = nullptr;
};
