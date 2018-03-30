#include "sendermanager.h"

SenderManager::SenderManager(QGridLayout* newLayout)
{
    mainLayout = newLayout;
    audioMode = QAudio::Mode::AudioInput;

    networkObject = new SoundSender();
    connect(networkObject, SIGNAL(connected()), this, SLOT(connected()));
    connect(networkObject, SIGNAL(disconnected()), this, SLOT(disconnected()));

    // initialization of widgets, connections and addind of widgets to mainLayout
    initAllWidgets();

    // set correct device widget's content
    deviceLabel->setText("Input device:");
    fillDeviceBox();
}

void SenderManager::initSpecificWidgets()
{
    setPreferredFormatButton = new QPushButton("Set preferred format for this device");
    ipLabel = new QLabel("your ip:");
    portLabel = new QLabel("your port:");
    ipLineEdit = new QLineEdit;
    portLineEdit = new QLineEdit;
    startButton = new QPushButton("Start sending");
    stopButton = new QPushButton("Stop sending");
    infoLabel = new QLabel("Wait for connections");

    mainLayout->addWidget(setPreferredFormatButton, 7, 0, 1, 2);
    mainLayout->addWidget(ipLabel, 8, 0);
    mainLayout->addWidget(ipLineEdit, 8, 1);
    mainLayout->addWidget(portLabel, 9, 0);
    mainLayout->addWidget(portLineEdit, 9, 1);
    mainLayout->addWidget(startButton, 10, 0);
    mainLayout->addWidget(stopButton, 10, 1);
    mainLayout->addWidget(infoLabel, 11, 0, 1, 2);

    ipLineEdit->setReadOnly(true);
    portLineEdit->setReadOnly(true);
    startButton->setEnabled(false);
    stopButton->setEnabled(false);
    infoLabel->setAlignment(Qt::AlignCenter);

    ipLineEdit->setText(networkObject->getHost().toString());
    portLineEdit->setText(QString::number(networkObject->getPort()));

    connect(setPreferredFormatButton, SIGNAL(clicked(bool)), this, SLOT(setPreferredFormat()));
    connect(startButton, SIGNAL(clicked(bool)), this, SLOT(handleStartButtonClicked()));
    connect(stopButton, SIGNAL(clicked(bool)), this, SLOT(handleStopButtonClicked()));
}

void SenderManager::connected()
{
    startButton->setEnabled(true);
    stopButton->setEnabled(false);
    infoLabel->setText("Receiver connected :)");
    handleStartButtonClicked();
}

void SenderManager::disconnected()
{
    changeLabelsToBoxes();
    startButton->setEnabled(false);
    stopButton->setEnabled(false);

    networkObject->stop();
    infoLabel->setText("Receiver disconnected");
}

void SenderManager::handleStartButtonClicked()
{
    QAudioFormat format = currentAudioFormat;
    while (!currentDeviceInfo->isFormatSupported(format))
    {
        qDebug() << "currentAudioFormat not supported, looking for the nearest";
        format = currentDeviceInfo->nearestFormat(format);
    }
    networkObject->updateParameters(*currentDeviceInfo, format);
    if (networkObject->start())
    {
        changeBoxesToLabels();
        startButton->setEnabled(false);
        stopButton->setEnabled(true);
        infoLabel->setText("Sending data");
    }
    else
        infoLabel->setText("Can not start with current settings :(");
}

void SenderManager::handleStopButtonClicked()
{
    changeLabelsToBoxes();
    startButton->setEnabled(true);
    stopButton->setEnabled(false);

    networkObject->stop();
    infoLabel->setText("Sending data is stopped");
}
