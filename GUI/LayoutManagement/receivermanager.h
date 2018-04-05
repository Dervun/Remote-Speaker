#pragma once
#include <QSlider>

#include "layoutmanager.h"
#include "../../Network/soundreceiver.h"

/*!
 * \brief The ReceiverManager class
 * Initializes specific widgets for the receiver and handle commands such as "Connect", "Mute", "Disable", etc.
 */
class ReceiverManager : public LayoutManager
{
    Q_OBJECT
public:
    explicit ReceiverManager(QGridLayout* newLayout);
    ~ReceiverManager();

private:
    /*!
     * \brief initSpecificWidgets
     * Initialize individual widgets for the receiver.
     * Presetting and adding them to the mainLayout.
     */
    void initSpecificWidgets();
    /*!
     * \brief changeEvent
     * \param event
     * Updates the text in widgets according to the new language if it changed,
     * then transfers control to the parent class.
     */
    void changeEvent(QEvent* event);

    SoundReceiver* soundReceiver = nullptr; /// It's network and audio component

    QLabel* reminderLabel = nullptr; /// Label instead of "set preffered format button"
    QPushButton* connectButton = nullptr;
    QPushButton* disconnectButton = nullptr;
    QLabel* bufferLimitLabel = nullptr; /// Label for bufferLimitSlider
    QSlider* bufferLimitSlider = nullptr; /// Slider to set the buffer limit in seconds
    QPushButton* muteButton = nullptr;
    QLabel* timeLabel = nullptr; /// Label to display how long the sound is played without loss of data

    quint64 lastTime = quint64(0);

private slots:
    void connected();
    void disconnected();
    void handleConnectButtonClicked();
    void handleDisconnectButtonClicked();
    void handleMuteButtonClicked();
    void handleBadConfigure();
    void handleGoodConfigure();
    void handleStopped();
    void handleBufferSizeChanged(int newLatency);
    void handleProcessedUsec(quint64 usec);
};

