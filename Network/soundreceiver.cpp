#include "soundreceiver.h"

SoundReceiver::SoundReceiver()
{
    deviceInfo = QAudioDeviceInfo::defaultOutputDevice();
}

void SoundReceiver::updateParameters(const QAudioDeviceInfo newDeviceInfo, const QAudioFormat newAudioFormat)
{
    deviceInfo = newDeviceInfo;
    Q_UNUSED(newAudioFormat);
}

bool SoundReceiver::start()
{

    // connect(udpSocket, SIGNAL(readyRead()), this, SLOT(readDatagrams()));
    // return udpSocket->bind(QHostAddress::Any, localPort, QUdpSocket::ShareAddress);
    return true;
}

void SoundReceiver::stop()
{
    // disconnect(udpSocket, SIGNAL(readyRead()), this, SLOT(readDatagrams()));
}

QHostAddress SoundReceiver::getHost()
{
    return senderSocket->peerAddress();
}

quint16 SoundReceiver::getPort()
{
    return senderSocket->peerPort();
}

bool SoundReceiver::tryToConnect(const QHostAddress address, const quint16 port)
{
    senderSocket = new QTcpSocket;
    senderSocket->connectToHost(address, port);
     if (!senderSocket->waitForConnected(3000))
         return false;

    connect(senderSocket, &QTcpSocket::readyRead, this, &SoundReceiver::configure);
    connect(senderSocket, &QTcpSocket::disconnected, this, &SoundReceiver::disconnected);
    connect(senderSocket, &QTcpSocket::disconnected, this, &SoundReceiver::handleDisconnected);
    connect(senderSocket, &QTcpSocket::disconnected, senderSocket, &QTcpSocket::deleteLater);

    emit connected();
    return true;
}

void SoundReceiver::disconnectFromHost()
{
    senderSocket->disconnectFromHost();
}

bool SoundReceiver::mute()
{
    if (audioOutput == nullptr)
        return false;
    if (audioOutput->volume() > 0)
    {
        audioOutput->setVolume(0);
        return true;
    }
    audioOutput->setVolume(1);
    return false;
}

void SoundReceiver::setBufferLimit(int number)
{
    int limitInMsec = 0;
    switch (number) {
    case 1:
        limitInMsec = 10;
        break;
    case 2:
        limitInMsec = 50;
        break;
    case 3:
        limitInMsec = 100;
        break;
    case 4:
        limitInMsec = 200;
        break;
    case 5:
        limitInMsec = 300;
        break;
    case 6:
        limitInMsec = 500;
        break;
    case 7:
        limitInMsec = 750;
        break;
    case 8:
        limitInMsec = 1000;
        break;
    default:
        limitInMsec = 2000;
        break;
    }
    // inf * 8 = sampleRate * sampleSize * countOfChannels * time  / 1000
    // where inf - quantity of saved sound in bytes, 8 - count of bits in byte, 1000 - count of msec in sec
    // Therefore, inf = sampleRate * sampleSize * countOfChannels * time  / 8000
    int limitInBytes = (audioFormat.sampleRate() * audioFormat.sampleSize() * audioFormat.channelCount() * limitInMsec) / 8000;
    qDebug() << "limitInBytes =" << limitInBytes;
    bufferLimit = std::max(limitInBytes, bufferSize  * 2);
    qDebug() << "bufferLimit =" << bufferLimit;
    limitInMsec = bufferLimit * 8000 / (audioFormat.sampleRate() * audioFormat.sampleSize() * audioFormat.channelCount());
    emit bufferSizeChanged(limitInMsec);
}

SoundReceiver::~SoundReceiver()
{
    if (audioOutput)
        delete audioOutput;
}

void SoundReceiver::readDatagrams()
{
    if (!configurationWasRead)
        configure();
    QByteArray* temporaryBuffer = new QByteArray;
    qDebug() << "serverSocket->bytesAvailable() ==" << senderSocket->bytesAvailable();
    if (senderSocket->bytesAvailable() > 0)
    {
        temporaryBuffer->append(senderSocket->readAll());
        if (QString::fromLocal8Bit(*temporaryBuffer).contains(QString("stop!!!")))
        {
            qDebug() << "stop!!!";
            disconnect(senderSocket, &QTcpSocket::readyRead, this, &SoundReceiver::readDatagrams);
            connect(senderSocket, &QTcpSocket::readyRead, this, &SoundReceiver::configure);
            configurationWasRead = false;
            if (isGoodConfiguration && audioOutput)
            {
                audioOutput->stop();
                delete audioOutput;
                audioOutput = nullptr;
                buffer.clear();
                isGoodConfiguration = false;
            }
            emit stopped();
        }
        else if (isGoodConfiguration)
            buffer.append(*temporaryBuffer);
    }
    delete temporaryBuffer;
    qDebug() << "buffer.size() =" << buffer.size();
    if (audioOutput != nullptr && audioOutput->state() == QAudio::IdleState)
    {
        qint64 bytesWritten = audioDevice->write(buffer.left(bufferSize));
        buffer.remove(0, bufferSize);
        qDebug() << "bytesWritten =" << bytesWritten;
    }
}

void SoundReceiver::handleStateChanged(QAudio::State newState)
{
    switch (newState)
    {
    case QAudio::IdleState:
        qDebug() << "new state - IdleState, SoundReceiver finished playing (no more data)";
        break;

    case QAudio::StoppedState:
        if (audioOutput->error() != QAudio::NoError)
            qDebug() << "new state - StoppedState, SoundReceiver stopped with error code " << audioOutput->error();
        else
            qDebug() << "new state - StoppedState, SoundReceiver stopped without errors";
        break;

    case QAudio::SuspendedState:
        qDebug() << "new state - SuspendedState, SoundReceiver suspended...";
        break;

    default:
        qDebug() << "O'key, SoundReceiver working";
        break;
    }
}

void SoundReceiver::handleDisconnected()
{
    if (audioOutput)
    {
        audioOutput->stop();
        delete audioOutput;
        audioOutput = nullptr;
        buffer.clear();
    }
}

void SoundReceiver::configure()
{
    readConfiguration();

    audioOutput = new QAudioOutput(deviceInfo, audioFormat);
    connect(audioOutput, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));

    disconnect(senderSocket, &QTcpSocket::readyRead, this, &SoundReceiver::configure);
    connect(senderSocket, &QTcpSocket::readyRead, this, &SoundReceiver::readDatagrams);

    audioDevice = audioOutput->start(); // Don't  need to destroy. "The pointer will become invalid after the stream is stopped or if you start another stream."
    if (audioOutput->state() == QAudio::StoppedState)
    {
        isGoodConfiguration = false;
        emit badConfigure();
        return;
    }
    // inf * 8 = sampleRate * sampleSize * countOfChannels * time  / 1000
    // where inf - quantity of saved sound in bytes, 8 - count of bits in byte, 1000 - count of msec in sec
    // Therefore, time = inf * 8 * 1000 / (sampleRate * sampleSize * countChannels)
    bufferSize = audioOutput->periodSize();
    qDebug() << "bufferSize =" << bufferSize;
    int timeForNotify = (bufferSize * 8000) / (audioFormat.sampleRate() * audioFormat.sampleSize() * audioFormat.channelCount());
    qDebug() << "timeForNotify =" << timeForNotify;
    audioOutput->setNotifyInterval(timeForNotify);
    connect(audioOutput, SIGNAL(notify()), this, SLOT(writeDataToDevice()));

    qDebug() << "audioOutput->notifyInterval() =" << audioOutput->notifyInterval();
    qDebug() << "bufferSize =" << bufferSize;
    qDebug() << "audioDevice->openMode() == " << audioDevice->openMode();

    emit goodConfigure();
    isGoodConfiguration = true;
}

void SoundReceiver::writeDataToDevice()
{
    if (audioOutput != nullptr)
    {
        if (buffer.size() > bufferLimit)
            truncateBuffer();
        audioDevice->write(buffer.left(bufferSize));
        qDebug() << "periodSize() =" << audioOutput->periodSize() << "buffer.size() =" << buffer.size();
        qDebug() << "processedUSecs() =" << audioOutput->processedUSecs();
        buffer.remove(0, bufferSize);
    }
    emit processedUsec(audioOutput->processedUSecs());
}

void SoundReceiver::truncateBuffer()
{
    buffer = buffer.right(bufferLimit);
    // smart truncate, now is bad
    /*
    // if buffer too large we take only last 1.125 of data
    if (buffer.size() > bufferLimit * 2)
        buffer = buffer.right(bufferLimit * 9 / 8);

    int bytesInMsec = (audioFormat.sampleRate() * audioFormat.sampleSize() * audioFormat.channelCount()) / 8000;
    qDebug() << "bytesInSec =" << bytesInMsec;

    // in this loop we remove each eighth msec until size of buffer correct or each second msec from buffer was removed
    for (int i = 0; buffer.size() > bufferLimit || i < 4; i++)
    {
        int shift = 0;
        switch (i)
        {
        case 0:
            shift = 1;
            break;
        case 1:
            shift = 4;
            break;
        case 2:
            shift = 2;
            break;
        default:
            shift = 4;
            break;
        }
        int currentPosition = buffer.size() - bytesInMsec * shift;
        while (currentPosition >= 0)
        {
            buffer.remove(currentPosition, bytesInMsec);
            currentPosition -= (8 - i) * bytesInMsec;
        }
    }
    */
}

void SoundReceiver::readConfiguration()
{
    QString currentSymbol = QString::fromLocal8Bit(senderSocket->read(2));
    while (currentSymbol != QString("{{"))
        currentSymbol = QString::fromLocal8Bit(senderSocket->read(2));

    currentSymbol = QString::fromLocal8Bit(senderSocket->read(1));
    QString sampleRate = QString("");
    while (currentSymbol != QString(";"))
    {
        if (currentSymbol == QString("{"))
        {
            currentSymbol = QString::fromLocal8Bit(senderSocket->read(1));
            continue;
        }
        sampleRate.append(currentSymbol);
        currentSymbol = QString::fromLocal8Bit(senderSocket->read(1));
    }
    qDebug() << "sampleRate =" << sampleRate;
    audioFormat.setSampleRate(sampleRate.toInt());

    currentSymbol = QString::fromLocal8Bit(senderSocket->read(1));
    QString channelCount = QString("");
    while (currentSymbol != QString(";"))
    {
        channelCount.append(currentSymbol);
        currentSymbol = QString::fromLocal8Bit(senderSocket->read(1));
    }
    qDebug() << "channelCount =" << channelCount;
    audioFormat.setChannelCount(channelCount.toInt());

    currentSymbol = QString::fromLocal8Bit(senderSocket->read(1));
    QString sampleSize = QString("");
    while (currentSymbol != QString(";"))
    {
        sampleSize.append(currentSymbol);
        currentSymbol = QString::fromLocal8Bit(senderSocket->read(1));
    }
    qDebug() << "sampleSize =" << sampleSize;
    audioFormat.setSampleSize(sampleSize.toInt());

    currentSymbol = QString::fromLocal8Bit(senderSocket->read(1));
    QString audioCodec = QString("");
    while (currentSymbol != QString(";"))
    {
        audioCodec.append(currentSymbol);
        currentSymbol = QString::fromLocal8Bit(senderSocket->read(1));
    }
    qDebug() << "audioCodec =" << audioCodec;
    audioFormat.setCodec(audioCodec);

    currentSymbol = QString::fromLocal8Bit(senderSocket->read(1));
    QString byteOrder = QString("");
    while (currentSymbol != QString(";"))
    {
        byteOrder.append(currentSymbol);
        currentSymbol = QString::fromLocal8Bit(senderSocket->read(1));
    }
    qDebug() << "byteOrder =" << byteOrder;
    if (byteOrder == QString("BigEndian"))
        audioFormat.setByteOrder(QAudioFormat::BigEndian);
    else
        audioFormat.setByteOrder(QAudioFormat::LittleEndian);

    currentSymbol = QString::fromLocal8Bit(senderSocket->read(1));
    QString sampleType = QString("");
    while (currentSymbol != QString(";"))
    {
        sampleType.append(currentSymbol);
        currentSymbol = QString::fromLocal8Bit(senderSocket->read(1));
    }
    qDebug() << "sampleType =" << sampleType;
    if (sampleType == QString("SignedInt"))
        audioFormat.setSampleType(QAudioFormat::SignedInt);
    else if (sampleType == QString("UnSignedInt"))
        audioFormat.setSampleType(QAudioFormat::UnSignedInt);
    else if (sampleType == QString("Float"))
        audioFormat.setSampleType(QAudioFormat::Float);
    else
        audioFormat.setSampleType(QAudioFormat::Unknown);

    configurationWasRead = true;
}
