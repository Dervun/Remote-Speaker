#include "receivermanager.h"

ReceiverManager::ReceiverManager(QGridLayout *newLayout)
{
    mainLayout = newLayout;
    audioMode = QAudio::Mode::AudioOutput;

    networkObject = new SoundReceiver();
    connect(networkObject, SIGNAL(connected()), this, SLOT(connected()));
    connect(networkObject, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(networkObject, SIGNAL(badConfigure()), this, SLOT(handleBadConfigure()));
    connect(networkObject, SIGNAL(goodConfigure()), this, SLOT(handleGoodConfigure()));
    connect(networkObject, SIGNAL(stopped()), this, SLOT(handleStopped()));
    connect(networkObject, SIGNAL(bufferSizeChanged(int)), this, SLOT(handleBufferSizeChanged(int)));
    connect(networkObject, SIGNAL(processedUsec(quint64)), this, SLOT(handleProcessedUsec(quint64)));

    // initialization of widgets, connections and addind of widgets to mainLayout
    initAllWidgets();

    // set correct device widget's content
    deviceLabel->setText("Output device:");
    fillDeviceBox();
}

void ReceiverManager::initSpecificWidgets()
{
    reminderLabel = new QLabel("You can see supported settings above");
    ipLabel = new QLabel("Enter ip:");
    portLabel = new QLabel("Enter port:");
    ipLineEdit = new QLineEdit;
    portLineEdit = new QLineEdit;
    connectButton = new QPushButton("Connect");
    disconnectButton = new QPushButton("Disconnect");
    infoLabel = new QLabel("Enter ip and port of sender and press \"Connect\" :)");
    timeLabel = new QLabel(QString("Sound plays for %1 seconds").arg(QString::number(0)));

    muteButton = new QPushButton("Mute");
    bufferLimitLabel = new QLabel("Buffer size:");
    bufferLimitSlider = new QSlider(Qt::Horizontal);

    mainLayout->addWidget(reminderLabel, 7, 0, 1, 2);
    mainLayout->addWidget(bufferLimitLabel, 8, 0);
    mainLayout->addWidget(bufferLimitSlider, 8, 1);
    bufferLimitLabel->hide();
    bufferLimitSlider->hide();
    mainLayout->addWidget(ipLabel, 8, 0);
    mainLayout->addWidget(ipLineEdit, 8, 1);
    mainLayout->addWidget(muteButton, 9, 0, 1, 2);
    muteButton->hide();
    mainLayout->addWidget(portLabel, 9, 0);
    mainLayout->addWidget(portLineEdit, 9, 1);
    mainLayout->addWidget(connectButton, 10, 0);
    mainLayout->addWidget(disconnectButton, 10, 1);
    mainLayout->addWidget(infoLabel, 11, 0, 1, 2);
    mainLayout->addWidget(timeLabel, 12, 0, 1, 2);

    connectButton->setEnabled(true);
    disconnectButton->setEnabled(false);
    reminderLabel->setAlignment(Qt::AlignCenter);
    infoLabel->setAlignment(Qt::AlignCenter);
    timeLabel->setAlignment(Qt::AlignCenter);
    timeLabel->setVisible(false);
    bufferLimitSlider->setRange(1, 9);
    bufferLimitSlider->setValue(7);

    connect(connectButton, SIGNAL(clicked(bool)), this, SLOT(handleConnectButtonClicked()));
    connect(ipLineEdit, SIGNAL(returnPressed()), portLineEdit, SLOT(setFocus()));
    connect(portLineEdit,SIGNAL(returnPressed()), this, SLOT(handleConnectButtonClicked()));
    connect(disconnectButton, SIGNAL(clicked(bool)), this, SLOT(handleDisconnectButtonClicked()));
    connect(muteButton, SIGNAL(clicked(bool)), this, SLOT(handleMuteButtonClicked()));
    connect(bufferLimitSlider, SIGNAL(valueChanged(int)), networkObject, SLOT(setBufferLimit(int)));
}

void ReceiverManager::connected()
{
    networkObject->updateParameters(*currentDeviceInfo, currentAudioFormat);

    connectButton->setEnabled(false);
    disconnectButton->setEnabled(true);
    ipLineEdit->setReadOnly(true);
    portLineEdit->setReadOnly(true);
    infoLabel->setText("Connected to sender");

    ipLabel->hide();
    ipLineEdit->hide();
    bufferLimitLabel->show();
    bufferLimitSlider->show();
    portLabel->hide();
    portLineEdit->hide();
    muteButton->show();

    timeLabel->setVisible(true);
}

void ReceiverManager::disconnected()
{
    connectButton->setEnabled(true);
    disconnectButton->setEnabled(false);
    ipLineEdit->setReadOnly(false);
    portLineEdit->setReadOnly(false);
    infoLabel->setText("Disconnected from sender");

    bufferLimitLabel->hide();
    bufferLimitSlider->hide();
    ipLabel->show();
    ipLineEdit->show();
    muteButton->hide();
    portLabel->show();
    portLineEdit->show();

    timeLabel->setVisible(false);
}

void ReceiverManager::handleConnectButtonClicked()
{
    infoLabel->setText("Wait for connection with sender");
    qDebug() << "void ReceiverManager::handleConnectButtonClicked()\n"
             << "Trying to connect to address: " << QHostAddress(ipLineEdit->text())
             << ", and port: " << portLineEdit->text().toUShort();
    if (!networkObject->tryToConnect(QHostAddress(ipLineEdit->text()), portLineEdit->text().toUShort()))
        infoLabel->setText("Could not connect to sender");
}

void ReceiverManager::handleDisconnectButtonClicked()
{
    networkObject->disconnectFromHost();
}

void ReceiverManager::handleMuteButtonClicked()
{
    if (networkObject->mute())
        muteButton->setText("Unmute");
    else
        muteButton->setText("Mute");
}

void ReceiverManager::handleBadConfigure()
{
    infoLabel->setText("Unsupported configure of audio format.");
}

void ReceiverManager::handleGoodConfigure()
{
    infoLabel->setText("Receiving data :)");
    muteButton->setEnabled(true);
}

void ReceiverManager::handleStopped()
{
    infoLabel->setText("Receiving data has been suspended");
    muteButton->setEnabled(false);
}

void ReceiverManager::handleBufferSizeChanged(int newLatency)
{
    infoLabel->setText(QString("Now latency <= %1 msec").arg(QString::number(newLatency)));
}

void ReceiverManager::handleProcessedUsec(quint64 usec)
{
    quint64 sec = usec / 1000000;
    if (sec == lastTime)
        return;
    if (sec < 60)
        timeLabel->setText(QString("Sound plays for %1 seconds without loss of data").arg(QString::number(sec)));
    else if (sec < 3600)
        timeLabel->setText(QString("Sound plays for %1 m %2 s without loss of data").
                           arg(QString::number(sec / 60), QString::number(sec % 60)));
    else
        timeLabel->setText(QString("Sound plays for %1 h %2 m %3 s without loss of data").
                           arg(QString::number(sec / 3600), QString::number((sec % 3600) / 60), QString::number(sec % 60)));
    lastTime = sec;
}
