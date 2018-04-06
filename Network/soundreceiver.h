#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QAudioDeviceInfo>
#include <QIODevice>
#include <QAudioOutput>

/*!
 * \brief The SoundReceiver class
 * Class for receiving of sound from sender.
 */
class SoundReceiver : public QObject
{
    Q_OBJECT

public:
    explicit SoundReceiver();
    /*!
     * \brief updateInfo
     * Updates deviceInfo.
     * \param newDeviceInfo
     */
    void updateInfo(const QAudioDeviceInfo newDeviceInfo);
    /*!
     * \brief tryToConnect
     * \param address Ip address of sender
     * \param port Port of sender
     * \return true if successfuly connected to the sender, false otherwise
     */
    bool tryToConnect(const QHostAddress address,const quint16 port);
    /*!
     * \brief disconnectFromSender
     * Disconnects from the sender
     */
    void disconnectFromSender();
    /*!
     * \brief mute
     * \return true if sound level now is 0, false otherwise
     */
    bool mute();
    ~SoundReceiver();

signals:
    /*!
     * \brief connected
     * This signal is emitted when the sender is successfully connected
     */
    void connected();
    /*!
     * \brief disconnected
     * This signal is emitted when the sender disconnected
     */
    void disconnected();
    /*!
     * \brief badConfigure
     * This signal is emitted when receiver has received a bad configuration
     */
    void badConfigure();
    /*!
     * \brief goodConfigure
     * This signal is emitted when the receiver has received a good configuration
     */
    void goodConfigure();
    /*!
     * \brief stopped
     * This signal is emitted when the sender stops sending data
     */
    void stopped();
    /*!
     * \brief bufferSizeChanged
     * This signal is emitted when the buffer size changed
     * \param bufferSize It's new size of the buffer in milliseconds
     */
    void bufferSizeChanged(int bufferSize);
    /*!
     * \brief processedUsec
     * This signal is emitted when writeDataToDevice() function is called
     * \param usec Time of the amount of audio data processed since state was changed to Active state in microseconds.
     */
    void processedUsec(quint64 usec);

public slots:
    /*!
     * \brief setBufferLimit
     * Sets new limit of the buffer.
     * \param number Integer from 1 to 10 (or more) corresponding to a certain size in milliseconds
     */
    void setBufferLimit(int number);

private slots:
    void readDatagrams();
    void handleStateChanged(QAudio::State newState);
    void handleDisconnected();
    void writeDataToDevice();

private:
    void truncateBuffer();
    void handleStop();
    void handleStart(QByteArray temp);
    void preSettings();
    void readConfiguration(QByteArray temp);

    QAudioDeviceInfo deviceInfo;
    QAudioFormat audioFormat = QAudioFormat();
    QTcpSocket* senderSocket = nullptr;
    QAudioOutput* audioOutput = nullptr;
    QIODevice* audioDevice = nullptr;
    bool playingNow = false;
    bool firstTime = true;

    QByteArray buffer; /// Buffer for storing received data
    int bufferSize = 15392; /// size for writing to device
    int bufferLimit = bufferSize * 5; /// max buffer size
};

