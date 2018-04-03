#pragma once

#include <QObject>
#include <QAudioDeviceInfo>
#include <QTcpSocket>
#include <QTcpServer>
#include <QIODevice>
#include <QAudioInput>

/*!
 * \brief The SoundSender class
 * Class for sending of sound to receiver.
 */
class SoundSender : public QObject
{
    Q_OBJECT

public:
    explicit SoundSender();
    /*!
     * \brief updateInfo
     * Updates deviceInfo and .audioFormat
     * \param newDeviceInfo
     * \param newAudioFormat
     */
    void updateInfo(const QAudioDeviceInfo newDeviceInfo, const QAudioFormat newAudioFormat);
    /*!
     * \brief startSending
     * Sends current settings to the receiver, tries to start data transfer.
     * On fail sends "stop!!!" to the receiver.
     * \return true if the data transfer has started successfully, false otherwise
     */
    bool startSending();
    /*!
     * \brief stopSending
     * Stops the data transfer and send "stop!!!" to receiver.
     */
    void stopSending();
    /*!
     * \brief getHost
     * \return Returns ip address of the sender
     */
    QHostAddress getHost();
    /*!
     * \brief getPort
     * \return Returns port address of the sender
     */
    quint16 getPort();
    ~SoundSender();

signals:
    /*!
     * \brief connected
     * This signal is emitted when the receiver is successfully connected.
     */
    void connected();
    /*!
     * \brief disconnected
     * This signal is emitted when the receiver disconnected.
     */
    void disconnected();

private slots:
    /*!
     * \brief newConnection
     * Been called when the receiver the receiver is successfully connected.
     */
    void newConnection();
    /*!
     * \brief handleDisconnected
     * Been called when the receiver disconnected.
     */
    void handleDisconnected();

    void handleStateChanged(QAudio::State newState);
    void bytesWritten(qint64 quantity);

private:
    /*!
     * \brief tryToListen
     * \param address
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
