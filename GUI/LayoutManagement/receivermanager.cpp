#include <QHostAddress>

#include "receivermanager.h"

ReceiverManager::ReceiverManager(QGridLayout* newLayout)
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
    deviceLabel->setText(tr("Output device:"));
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
    reminderLabel = new QLabel(tr("You can see supported settings above"));
    ipLabel = new QLabel(tr("Enter ip:"));
    portLabel = new QLabel(tr("Enter port:"));
    ipLineEdit = new QLineEdit;
    portLineEdit = new QLineEdit;
    connectButton = new QPushButton(tr("Connect"));
    disconnectButton = new QPushButton(tr("Disconnect"));
    infoLabel = new QLabel(tr("Enter ip and port of sender and press \"Connect\" :)"));
    timeLabel = new QLabel(tr("Sound plays for %1 seconds").arg(QString::number(0)));

    muteButton = new QPushButton(tr("Mute"));
    bufferLimitLabel = new QLabel(tr("Buffer size:"));
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
    bufferLimitSlider->setRange(1, 10);
    bufferLimitSlider->setValue(5);

    connect(connectButton, SIGNAL(clicked(bool)), this, SLOT(handleConnectButtonClicked()));
    connect(ipLineEdit, SIGNAL(returnPressed()), portLineEdit, SLOT(setFocus()));
    connect(portLineEdit,SIGNAL(returnPressed()), this, SLOT(handleConnectButtonClicked()));
    connect(disconnectButton, SIGNAL(clicked(bool)), this, SLOT(handleDisconnectButtonClicked()));
    connect(muteButton, SIGNAL(clicked(bool)), this, SLOT(handleMuteButtonClicked()));
    connect(bufferLimitSlider, SIGNAL(valueChanged(int)), soundReceiver, SLOT(setBufferLimit(int)));
}

void ReceiverManager::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        deviceLabel->setText(tr("Output device:"));
        reminderLabel->setText(tr("You can see supported settings above"));
        ipLabel->setText(tr("Enter ip:"));
        portLabel->setText(tr("Enter port:"));
        connectButton->setText(tr("Connect"));
        disconnectButton->setText(tr("Disconnect"));
        infoLabel->setText(tr("Enter ip and port of sender and press \"Connect\" :)"));
        timeLabel->setText(tr("Sound plays for %1 seconds").arg(QString::number(0)));
        muteButton->setText(tr("Mute"));
        bufferLimitLabel->setText(tr("Buffer size:"));

        LayoutManager::changeEvent(event);
    }
    else
        QWidget::changeEvent(event);
}

void ReceiverManager::connected()
{
    soundReceiver->updateInfo(*currentDeviceInfo);

    connectButton->setEnabled(false);
    disconnectButton->setEnabled(true);
    ipLineEdit->setReadOnly(true);
    portLineEdit->setReadOnly(true);
    infoLabel->setText(tr("Connected to sender"));

    ipLabel->hide();
    ipLineEdit->hide();
    bufferLimitLabel->show();
    bufferLimitSlider->show();
    bufferLimitSlider->setValue(5);
    portLabel->hide();
    portLineEdit->hide();
    muteButton->show();

    timeLabel->setVisible(true);
    LayoutManager::connected();
}

void ReceiverManager::disconnected()
{
    connectButton->setEnabled(true);
    disconnectButton->setEnabled(false);
    ipLineEdit->setReadOnly(false);
    portLineEdit->setReadOnly(false);
    infoLabel->setText(tr("Disconnected from sender"));

    bufferLimitLabel->hide();
    bufferLimitSlider->hide();
    ipLabel->show();
    ipLineEdit->show();
    muteButton->hide();
    portLabel->show();
    portLineEdit->show();

    timeLabel->setVisible(false);
    LayoutManager::disconnected();
}

void ReceiverManager::handleConnectButtonClicked()
{
    infoLabel->setText(tr("Wait for connection with sender"));
    qDebug() << "void ReceiverManager::handleConnectButtonClicked()\n"
             << "Trying to connect to address: " << QHostAddress(ipLineEdit->text())
             << ", and port: " << portLineEdit->text().toUShort();
    if (!soundReceiver->tryToConnect(QHostAddress(ipLineEdit->text()), portLineEdit->text().toUShort()))
        infoLabel->setText(tr("Could not connect to sender"));
}

void ReceiverManager::handleDisconnectButtonClicked()
{
    soundReceiver->disconnectFromSender();
}

void ReceiverManager::handleMuteButtonClicked()
{
    if (soundReceiver->mute())
        muteButton->setText(tr("Unmute"));
    else
        muteButton->setText(tr("Mute"));
}

void ReceiverManager::handleBadConfigure()
{
    infoLabel->setText(tr("Unsupported configure of audio format"));
}

void ReceiverManager::handleGoodConfigure()
{
    infoLabel->setText(tr("Receiving data :)"));
    muteButton->setEnabled(true);
}

void ReceiverManager::handleStopped()
{
    infoLabel->setText(tr("Receiving data has been suspended"));
    muteButton->setEnabled(false);
    muteButton->setText(tr("Mute"));
}

void ReceiverManager::handleBufferSizeChanged(int newLatency)
{
    infoLabel->setText(tr("Now the size of the buffer = %1 msec").arg(QString::number(newLatency)));
}

void ReceiverManager::handleProcessedUsec(quint64 usec)
{
    quint64 sec = usec / 1000000;
    if (sec == lastTime)
        return;
    if (sec < 60)
        timeLabel->setText(tr("Sound plays for %1 seconds without loss of data").arg(QString::number(sec)));
    else if (sec < 3600)
        timeLabel->setText(tr("Sound plays for %1 m %2 s without loss of data").
                           arg(QString::number(sec / 60), QString::number(sec % 60)));
    else
        timeLabel->setText(tr("Sound plays for %1 h %2 m %3 s without loss of data").
                           arg(QString::number(sec / 3600), QString::number((sec % 3600) / 60), QString::number(sec % 60)));
    lastTime = sec;
}
