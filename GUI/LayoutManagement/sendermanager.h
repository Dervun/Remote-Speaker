#pragma once
#include "layoutmanager.h"
#include "../../Network/soundsender.h"
#include <QCheckBox>

/*!
 * \brief The SenderManager class
 * Initializes specific widgets for the sender and handle commands such as "Start", "Stop".
 */
class SenderManager : public LayoutManager
{
    Q_OBJECT
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
    /*!
     * \brief changeEvent
     * \param event
     * Updates the text in widgets according to the new language if it changed,
     * then transfers control to the parent class.
     */
    void changeEvent(QEvent* event);

    SoundSender* soundSender = nullptr; /// It's network and audio component

    QCheckBox* useCompressionCheckBox = nullptr;
    QPushButton* startButton = nullptr;
    QPushButton* stopButton = nullptr;
    
private slots:
    void connected();
    void disconnected();
    void handleStartButtonClicked();
    void handleStopButtonClicked();
};

