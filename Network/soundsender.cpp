#include <QNetworkInterface>
#include <QBitArray>

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

void SoundSender::updateInfo(const QAudioDeviceInfo newDeviceInfo, const QAudioFormat newAudioFormat, bool useCompression)
{
    deviceInfo = newDeviceInfo;
    audioFormat = newAudioFormat;
    this->useCompression = useCompression;
}

bool SoundSender::startSending()
{
    receiverSocket->setSocketOption(QAbstractSocket::LowDelayOption, QVariant(1));

    audioInput = new QAudioInput(deviceInfo, audioFormat);
//    connect(audioInput, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));
    audioDevice = audioInput->start(); // Don't  need to destroy. "The pointer will become invalid after the stream is stopped or if you start another stream."
    qDebug() << "audioDevice->isOpen()" << audioDevice->isOpen() << "audioDevice->openMode()" << audioDevice->openMode();
    receiverSocket->write(getCurrentSettings());
    if (useCompression)
        connect(audioDevice, SIGNAL(readyRead()), this, SLOT(writeDataWithCompression()));
    else
        connect(audioDevice, SIGNAL(readyRead()), this, SLOT(writeDataWithoutCompression()));


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
    emit connected();
}

void SoundSender::handleDisconnected()
{
    stopSending();
}

void SoundSender::writeDataWithCompression()
{
    qDebug() << "audioInput->bytesReady() =" << audioInput->bytesReady();
    if (audioInput->bytesReady() < audioInput->periodSize())
        return;
    QByteArray buffer = audioDevice->read(qint64(audioInput->periodSize()));
    qDebug() << "buffer.size() =" << buffer.size();
    buffer = qCompress(buffer, compressionPower);

    int sizeOfBlock = buffer.size();
    qDebug() << "sizeOfBlock =" << sizeOfBlock;

    QBitArray inBits = QBitArray(16);
    for (int i = 15; sizeOfBlock > 0 && i >= 0; i--)
    {
        inBits[i] = sizeOfBlock % 2;
        sizeOfBlock = sizeOfBlock >> 1;
    }
    qDebug() << inBits;
    // Resulting byte array
    QByteArray inBytes = QByteArray(2, (char) 0);
    // Convert from QBitArray to QByteArray
    for (int i = 0; i < 16; i++)
        inBytes[i / 8] = (inBytes.at(i / 8) | ((inBits[i] ? 1 : 0) << (7 - (i % 8))));

    // first 2 bytes, they contsains size of compressed block
    buffer.prepend(inBytes);
    quint64 bytesWritten = receiverSocket->write(buffer);
    qDebug() << "bytesWritten =" << bytesWritten;
}

void SoundSender::writeDataWithoutCompression()
{
    qDebug() << "audioInput->bytesReady() =" << audioInput->bytesReady();
    QByteArray buffer;
    buffer = audioDevice->readAll();
    qDebug() << "buffer.size() =" << buffer.size();
    quint64 bytesWritten = receiverSocket->write(buffer);
    qDebug() << "bytesWritten =" << bytesWritten;
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
    settings.append(QString("start!!!").toLocal8Bit());
    if (useCompression)
        settings.append(QString("DATACOMPRESSION"));
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
