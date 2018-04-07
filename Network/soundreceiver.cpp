#include <QHostAddress>
#include <QBitArray>

#include "soundreceiver.h"

SoundReceiver::SoundReceiver()
{
    deviceInfo = QAudioDeviceInfo::defaultOutputDevice();
}

void SoundReceiver::updateInfo(const QAudioDeviceInfo newDeviceInfo)
{
    deviceInfo = newDeviceInfo;
}

bool SoundReceiver::tryToConnect(const QHostAddress address, const quint16 port)
{
    senderSocket = new QTcpSocket;
    senderSocket->connectToHost(address, port);
     if (!senderSocket->waitForConnected(3000))
         return false;

    connect(senderSocket, SIGNAL(readyRead()), this, SLOT(readDatagrams()));
    connect(senderSocket, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
    connect(senderSocket, SIGNAL(disconnected()), this, SLOT(handleDisconnected()));
    connect(senderSocket, SIGNAL(disconnected()), senderSocket, SLOT(deleteLater()));

    emit connected();
    return true;
}

void SoundReceiver::disconnectFromSender()
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
    int limitInMsec = 3000;
    switch (number) {
    case 1:
        limitInMsec = 50;
        break;
    case 2:
        limitInMsec = 100;
        break;
    case 3:
        limitInMsec = 150;
        break;
    case 4:
        limitInMsec = 200;
        break;
    case 5:
        limitInMsec = 300;
        break;
    case 6:
        limitInMsec = 400;
        break;
    case 7:
        limitInMsec = 500;
        break;
    case 8:
        limitInMsec = 750;
        break;
    case 9:
        limitInMsec = 1000;
        break;
    }
    // inf * 8 = sampleRate * sampleSize * countOfChannels * time  / 1000
    // where inf - quantity of saved sound in bytes, 8 - count of bits in byte, 1000 - count of msec in sec
    // Therefore, inf = sampleRate * sampleSize * countOfChannels * time  / 8000
    int limitInBytes = (audioFormat.sampleRate() / 8000) * audioFormat.sampleSize() * audioFormat.channelCount() * limitInMsec;
    qDebug() << "limitInBytes =" << limitInBytes;
    bufferLimit = std::max(limitInBytes, bufferSize);
    qDebug() << "bufferLimit =" << bufferLimit;
    limitInMsec = (int) (((double) bufferLimit / (audioFormat.sampleRate() * audioFormat.sampleSize() * audioFormat.channelCount())) * 8000);
    emit bufferSizeChanged(limitInMsec);
}

SoundReceiver::~SoundReceiver()
{
    if (audioOutput)
        delete audioOutput;
}

void SoundReceiver::readDatagrams()
{
    qDebug() << "readDatagrams()";
    QByteArray temporaryBuffer;
    qDebug() << "serverSocket->bytesAvailable() ==" << senderSocket->bytesAvailable();
    if (senderSocket->bytesAvailable() > 0)
    {
        temporaryBuffer = senderSocket->readAll();
        if (playingNow)
        {
            if (QString::fromLocal8Bit(temporaryBuffer).contains(QString("stop!!!")))
            {
                handleStop();
                return;
            }
        }
        else
        {
            if (QString::fromLocal8Bit(temporaryBuffer).contains(QString("start!!!")))
            {
                handleStart(temporaryBuffer);
                return;
            }
        }
        buffer.append(temporaryBuffer);
    }
    qDebug() << "buffer.size() =" << buffer.size();
    // the first time it's necessary to write data
    if (firstTime && buffer.size() >= bufferSize)
    {
        writeDataToDevice();
        firstTime = false;
        return;
    }
    // if something goes wrong, the data will still be record
    if (buffer.size() >= bufferLimit)
    {
        qDebug() << "if (audioOutput != nullptr && buffer.size() > bufferLimit)";
        writeDataToDevice();
    }
}

void SoundReceiver::readCompressedDatagrams()
{
    qDebug() << "readCompressedDatagrams()";
    QByteArray temporaryBuffer;
    qDebug() << "serverSocket->bytesAvailable() ==" << senderSocket->bytesAvailable();
    while (senderSocket->bytesAvailable() > 0)
    {
        // Peek in the next 1000 bytes
        temporaryBuffer = senderSocket->peek(65536);
        if (playingNow)
        {
            if (QString::fromLocal8Bit(temporaryBuffer).contains(QString("stop!!!")))
            {
                qDebug() << "if (QString::fromLocal8Bit(temporaryBuffer).contains(QString(stop!!!)";
                senderSocket->read(temporaryBuffer.size());
                handleStop();
                return;
            }
        }
        else
        {
            if (QString::fromLocal8Bit(temporaryBuffer).contains(QString("start!!!")))
            {
                qDebug() << "if (QString::fromLocal8Bit(temporaryBuffer).contains(QString(start!!!)))";
                senderSocket->read(temporaryBuffer.size());
                handleStart(temporaryBuffer);
                return;
            }
        }
        if (currentSizeOfBlock == 0)
        {
            // read first 2 bytes, they contsains size of compressed block
            currentSizeOfBlock = twoBytesToInt(temporaryBuffer.left(2));
            qDebug() << "currentSizeOfBlock =" << currentSizeOfBlock;
        }
        // if not all data was fully received
        if (temporaryBuffer.size() < currentSizeOfBlock + 2)
        {
            qDebug() << "not enough data";
            return;
        }
        else
        {
            temporaryBuffer = senderSocket->read(2 + currentSizeOfBlock);
            qDebug() << "temporaryBuffer.size() =" << temporaryBuffer.size();
            temporaryBuffer = qUncompress(temporaryBuffer.right(currentSizeOfBlock));
            qDebug() << "temporaryBuffer.size() =" << temporaryBuffer.size();
            buffer.append(temporaryBuffer);
            currentSizeOfBlock = 0;
        }
    }
    qDebug() << "buffer.size() =" << buffer.size();
    // the first time it's necessary to write data
    if (firstTime && buffer.size() >= bufferSize)
    {
        writeDataToDevice();
        firstTime = false;
        return;
    }
    // if something goes wrong, the data will still be record
    if (buffer.size() >= bufferLimit)
    {
        qDebug() << "if (audioOutput != nullptr && buffer.size() > bufferLimit)";
        writeDataToDevice();
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
    handleStop();
}

void SoundReceiver::preSettings()
{
    audioOutput = new QAudioOutput(deviceInfo, audioFormat);
//    connect(audioOutput, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));

    audioDevice = audioOutput->start(); // Don't  need to destroy. "The pointer will become invalid after the stream is stopped or if you start another stream."
    if (audioOutput->state() == QAudio::StoppedState)
    {
        playingNow = false;
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
    currentSizeOfBlock = 0;
    connect(audioOutput, SIGNAL(notify()), this, SLOT(writeDataToDevice()));

    qDebug() << "audioOutput->notifyInterval() =" << audioOutput->notifyInterval();
    qDebug() << "audioDevice->openMode() == " << audioDevice->openMode();

    emit goodConfigure();
    playingNow = true;
}

void SoundReceiver::writeDataToDevice()
{
    qDebug() << "void SoundReceiver::writeDataToDevice()";
    if (buffer.size() > bufferLimit)
        truncateBuffer();
    if (audioOutput != nullptr)
    {
        audioDevice->write(buffer.left(bufferSize));
        qDebug() << "periodSize() =" << audioOutput->periodSize() << "buffer.size() =" << buffer.size();
        qDebug() << "processedUSecs() =" << audioOutput->processedUSecs();
        buffer.remove(0, bufferSize);
        emit processedUsec(audioOutput->processedUSecs());
    }
}

void SoundReceiver::truncateBuffer()
{
    qDebug() << "void SoundReceiver::truncateBuffer()";
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

void SoundReceiver::handleStop()
{
    qDebug() << "stop!!!";
    if (audioOutput != nullptr)
    {
        audioOutput->stop();
        delete audioOutput;
        audioOutput = nullptr;
    }

    disconnect(senderSocket, SIGNAL(readyRead()), this, SLOT(readDatagrams()));
    disconnect(senderSocket, SIGNAL(readyRead()), this, SLOT(readCompressedDatagrams()));

    connect(senderSocket, SIGNAL(readyRead()), this, SLOT(readDatagrams()));

    buffer.clear();
    senderSocket->readAll();
    playingNow = false;
    firstTime = true;
    emit stopped();
}

void SoundReceiver::handleStart(QByteArray temp)
{
    qDebug() << "start!!!";
    firstTime = true;
    readConfiguration(temp);
    preSettings();
}

void SoundReceiver::readConfiguration(QByteArray temp)
{
    const char separator = ';';
    qDebug() << QString::fromLocal8Bit(temp);
    int k = 0;
    while (temp.mid(k, 8) != QString("start!!!").toLocal8Bit() && k < temp.size())
        k++;
    if (k + 8 >= temp.size())
    {
        playingNow = false;
        emit badConfigure();
        return;
    }
    temp.remove(0, k + 8);

    disconnect(senderSocket, SIGNAL(readyRead()), this, SLOT(readDatagrams()));
    disconnect(senderSocket, SIGNAL(readyRead()), this, SLOT(readCompressedDatagrams()));

    if (temp.left(15) == QString("DATACOMPRESSION").toLocal8Bit())
    {
        dataCompressionWasUsed = true;
        qDebug() << "dataCompressionWasUsed";
        temp.remove(0, 15);

        connect(senderSocket, SIGNAL(readyRead()), this, SLOT(readCompressedDatagrams()));
    }
    else
    {
        connect(senderSocket, SIGNAL(readyRead()), this, SLOT(readDatagrams()));
        dataCompressionWasUsed = false;
    }

    k = 0;
    while (temp.at(k) != separator)
        k++;
    QString sampleRate = QString::fromLocal8Bit(temp.left(k));
    qDebug() << "sampleRate =" << sampleRate;
    audioFormat.setSampleRate(sampleRate.toInt());
    temp.remove(0, k + 1);

    k = 0;
    while (temp.at(k) != separator)
        k++;
    QString channelCount = QString::fromLocal8Bit(temp.left(k));
    qDebug() << "channelCount =" << channelCount;
    audioFormat.setChannelCount(channelCount.toInt());
    temp.remove(0, k + 1);

    k = 0;
    while (temp.at(k) != separator)
        k++;
    QString sampleSize = QString::fromLocal8Bit(temp.left(k));
    qDebug() << "sampleSize =" << sampleSize;
    audioFormat.setSampleSize(sampleSize.toInt());
    temp.remove(0, k + 1);

    k = 0;
    while (temp.at(k) != separator)
        k++;
    QString audioCodec = QString::fromLocal8Bit(temp.left(k));
    qDebug() << "audioCodec =" << audioCodec;
    audioFormat.setCodec(audioCodec);
    temp.remove(0, k + 1);

    k = 0;
    while (temp.at(k) != separator)
        k++;
    QString byteOrder = QString::fromLocal8Bit(temp.left(k));
    qDebug() << "byteOrder =" << byteOrder;
    if (byteOrder == QString("BigEndian"))
        audioFormat.setByteOrder(QAudioFormat::BigEndian);
    else
        audioFormat.setByteOrder(QAudioFormat::LittleEndian);
    temp.remove(0, k + 1);

    k = 0;
    while (temp.at(k) != separator)
        k++;
    QString sampleType = QString::fromLocal8Bit(temp.left(k));
    qDebug() << "sampleType =" << sampleType;
    if (sampleType == QString("SignedInt"))
        audioFormat.setSampleType(QAudioFormat::SignedInt);
    else if (sampleType == QString("UnSignedInt"))
        audioFormat.setSampleType(QAudioFormat::UnSignedInt);
    else if (sampleType == QString("Float"))
        audioFormat.setSampleType(QAudioFormat::Float);
    else
        audioFormat.setSampleType(QAudioFormat::Unknown);
    temp.clear();
}

int SoundReceiver::twoBytesToInt(QByteArray twoBytes)
{
    QBitArray sizeInBits = QBitArray(16);
    for(int i = 0; i < 8; i++)
        sizeInBits.setBit(i, twoBytes.at(0) & (1 << (7 - i)));
    for(int i = 0; i < 8; i++)
        sizeInBits.setBit(8 + i, twoBytes.at(1) & (1 << (7 - i)));
    qDebug() << sizeInBits;

    int result = 0;
    int multiplier = 1;
    for (int i = 15; i > 0; i--)
    {
        result += (sizeInBits.at(i) ? 1 : 0) * multiplier;
        multiplier = multiplier << 1;
    }
    return result;
}
