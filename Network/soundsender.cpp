#include <QNetworkInterface>

#include "soundsender.h"

SoundSender::SoundSender()
{
    server = new QTcpServer(this);
    foreach (const QHostAddress &address , QNetworkInterface::allAddresses())
    {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && !address.isLoopback())
            if (tryToListen(address))
                break;
    }
}

void SoundSender::updateInfo(const QAudioDeviceInfo newDeviceInfo, const QAudioFormat newAudioFormat)
{
    deviceInfo = newDeviceInfo;
    audioFormat = newAudioFormat;
}

bool SoundSender::startSending()
{
    receiverSocket->write(getCurrentSettings());
    receiverSocket->setSocketOption(QAbstractSocket::LowDelayOption, QVariant(1)); // Попробовать для уменьшения задержки

    audioInput = new QAudioInput(deviceInfo, audioFormat);
    connect(audioInput, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));
    audioInput->start(receiverSocket);

    qDebug() << "audioInput->periodSize() = " << audioInput->periodSize();
    if (audioInput->state() == QAudio::StoppedState)
    {
        stopSending();
        return false;
    }
    return true;
}

void SoundSender::stopSending()
{
    if (audioInput)
    {
        audioInput->stop();
        delete audioInput;
        audioInput = nullptr;
    }
    if (receiverSocket->isOpen())
        receiverSocket->write(QString("stop!!!").toLocal8Bit());
}

QHostAddress SoundSender::getHost()
{
    if (!server)
        return QHostAddress::Null;
    return server->serverAddress();
}

quint16 SoundSender::getPort()
{
    if (!server)
        return quint16(0);
    return server->serverPort();
}

SoundSender::~SoundSender()
{
    if (audioInput)
    {
        audioInput->stop();
        delete audioInput;
        audioInput = nullptr;
    }
    if (server)
        delete server;
}

void SoundSender::newConnection()
{
    if (!server->hasPendingConnections())
        return;
    receiverSocket = server->nextPendingConnection();
    connect(receiverSocket, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
    connect(receiverSocket, SIGNAL(disconnected()), receiverSocket, SLOT(deleteLater()));
//    connect(clientSocket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)));
    emit connected();
}

void SoundSender::handleStateChanged(QAudio::State newState)
{
    switch (newState)
    {
    case QAudio::StoppedState:
        if (audioInput->error() != QAudio::NoError)
            qDebug() << "SoundSender stopped with error code " << audioInput->error();
        else
            qDebug() << "SoundSender stopped without errors";
        break;

    case QAudio::ActiveState:
        qDebug() << "O'key, SoundSender started recording - read from audioInput";
        break;

    case QAudio::SuspendedState:
        qDebug() << "SoundSender changed state to SuspendedState";
        break;

    default:
        qDebug() << "SoundSender changed state to IdleState";
        break;
    }
}

void SoundSender::handleDisconnected()
{
    stopSending();
}

void SoundSender::bytesWritten(qint64 quantity)
{
    qDebug() << quantity << "bytes written";
    if (audioInput != nullptr)
        qDebug() << "processedUSecs()" << audioInput->processedUSecs();
}

bool SoundSender::tryToListen(const QHostAddress &address)
{
    if (!server->listen(address))
        return false;
    connect(server, SIGNAL(newConnection()),
            this, SLOT(newConnection()));
    return true;
}

QByteArray SoundSender::getCurrentSettings()
{
    QByteArray settings;
    settings.append(QString("{{{").toLocal8Bit());
    settings.append(QString::number(audioFormat.sampleRate()).toLocal8Bit());
    settings.append(QString(";").toLocal8Bit());
    settings.append(QString::number(audioFormat.channelCount()).toLocal8Bit());
    settings.append(QString(";").toLocal8Bit());
    settings.append(QString::number(audioFormat.sampleSize()).toLocal8Bit());
    settings.append(QString(";").toLocal8Bit());
    settings.append(audioFormat.codec().toLocal8Bit());
    settings.append(QString(";").toLocal8Bit());
    if (audioFormat.byteOrder() == QAudioFormat::LittleEndian)
        settings.append(QString("LittleEndian").toLocal8Bit());
    else
        settings.append(QString("BigEndian").toLocal8Bit());
    settings.append(QString(";").toLocal8Bit());
    if (audioFormat.sampleType() == QAudioFormat::SignedInt)
        settings.append(QString("SignedInt").toLocal8Bit());
    else if (audioFormat.sampleType() == QAudioFormat::SignedInt)
        settings.append(QString("SignedInt").toLocal8Bit());
    else if (audioFormat.sampleType() == QAudioFormat::UnSignedInt)
        settings.append(QString("UnSignedInt").toLocal8Bit());
    else if (audioFormat.sampleType() == QAudioFormat::Float)
        settings.append(QString("Float").toLocal8Bit());
    else
        settings.append(QString("Unknown").toLocal8Bit());
    settings.append(QString(";").toLocal8Bit());
    return settings;
}
