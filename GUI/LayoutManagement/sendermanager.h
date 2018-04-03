#pragma once
#include "layoutmanager.h"
#include "../../Network/soundsender.h"

/*!
 * \brief The SenderManager class
 * Initializes specific widgets for the sender and handle commands such as "Start", "Stop".
 */
class SenderManager : public LayoutManager
{
public:
    explicit SenderManager(QGridLayout* newLayout);
    ~SenderManager();

private:
    /*!
     * \brief initSpecificWidgets
     * Initialize individual widgets for the sender.
     * Presetting and adding them to the mainLayout.
     */
    void initSpecificWidgets();

    SoundSender* soundSender = nullptr; /// It's network and audio component

    QPushButton* setPreferredFormatButton = nullptr;
    QPushButton* startButton = nullptr;
    QPushButton* stopButton = nullptr;
    
private slots:
    void connected();
    void disconnected();
    void handleStartButtonClicked();
    void handleStopButtonClicked();
};

