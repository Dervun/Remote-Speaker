#pragma once
#include <QSlider>

#include "layoutmanager.h"
#include "../../Network/soundreceiver.h"

class ReceiverManager : public LayoutManager
{
public:
    explicit ReceiverManager(QGridLayout* newLayout);
    ~ReceiverManager();

private:
    void initSpecificWidgets();

    SoundReceiver* soundReceiver = nullptr;

    QLabel* reminderLabel = nullptr;
    QPushButton* connectButton = nullptr;
    QPushButton* disconnectButton = nullptr;
    QLabel* bufferLimitLabel = nullptr;
    QSlider* bufferLimitSlider = nullptr;
    QPushButton* muteButton = nullptr;
    QLabel* timeLabel = nullptr;

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

