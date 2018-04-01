#include <QHostAddress>

#include "receivermanager.h"

ReceiverManager::ReceiverManager(QGridLayout *newLayout)
{
    mainLayout = newLayout;
    audioMode = QAudio::Mode::AudioOutput;

    soundReceiver = new SoundReceiver();
    connect(soundReceiver, SIGNAL(connected()), this, SLOT(connected()));
    connect(soundReceiver, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(soundReceiver, SIGNAL(badConfigure()), this, SLOT(handleBadConfigure()));
    connect(soundReceiver, SIGNAL(goodConfigure()), this, SLOT(handleGoodConfigure()));
    connect(soundReceiver, SIGNAL(stopped()), this, SLOT(handleStopped()));
    connect(soundReceiver, SIGNAL(bufferSizeChanged(int)), this, SLOT(handleBufferSizeChanged(int)));
    connect(soundReceiver, SIGNAL(processedUsec(quint64)), this, SLOT(handleProcessedUsec(quint64)));

    // initialization of widgets, connections and addind of widgets to mainLayout
    initAllWidgets();

    // set correct device widget's content
    deviceLabel->setText("Output device:");
    fillDeviceBox();
}

ReceiverManager::~ReceiverManager()
{
    mainLayout->removeWidget(reminderLabel);
    mainLayout->removeWidget(ipLabel);
    mainLayout->removeWidget(portLabel);
    mainLayout->removeWidget(ipLineEdit);
    mainLayout->removeWidget(portLineEdit);
    mainLayout->removeWidget(connectButton);
    mainLayout->removeWidget(disconnectButton);
    mainLayout->removeWidget(infoLabel);
    mainLayout->removeWidget(timeLabel);

    mainLayout->removeWidget(muteButton);
    mainLayout->removeWidget(bufferLimitLabel);
    mainLayout->removeWidget(bufferLimitSlider);

    delete reminderLabel;
    delete ipLabel;
    delete portLabel;
    delete ipLineEdit;
    delete portLineEdit;
    delete connectButton;
    delete disconnectButton;
    delete infoLabel;
    delete timeLabel;

    delete muteButton;
    delete bufferLimitLabel;
    delete bufferLimitSlider;
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
    connect(bufferLimitSlider, SIGNAL(valueChanged(int)), soundReceiver, SLOT(setBufferLimit(int)));
}

void ReceiverManager::connected()
{
    soundReceiver->updateInfo(*currentDeviceInfo);

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
    if (!soundReceiver->tryToConnect(QHostAddress(ipLineEdit->text()), portLineEdit->text().toUShort()))
        infoLabel->setText("Could not connect to sender");
}

void ReceiverManager::handleDisconnectButtonClicked()
{
    soundReceiver->disconnectFromHost();
}

void ReceiverManager::handleMuteButtonClicked()
{
    if (soundReceiver->mute())
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
    infoLabel->setText(QString("Now delay <= %1 msec").arg(QString::number(newLatency)));
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
