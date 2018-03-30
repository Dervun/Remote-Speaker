#pragma once
#include "layoutmanager.h"
#include "../../Network/soundsender.h"

class SenderManager : public LayoutManager
{
public:
    explicit SenderManager(QGridLayout* newLayout);
    ~SenderManager(){}

private:
    void initSpecificWidgets();

    NetworkObject* networkObject = nullptr;

    QPushButton* setPreferredFormatButton = nullptr;
    QPushButton* startButton = nullptr;
    QPushButton* stopButton = nullptr;
    
private slots:
    void connected();
    void disconnected();
    void handleStartButtonClicked();
    void handleStopButtonClicked();
};

