#include "layoutmanager.h"

LayoutManager::~LayoutManager()
{
    removeWidgetsFromLayout();
    deleteWidgets();
}

void LayoutManager::setPreferredFormat()
{
    QAudioFormat prefferedFormat = currentDeviceInfo->preferredFormat();

    // try to find correct format
    int sampleRate = sampleRateComboBox->findData(QVariant(48000));
    int channelCount = channelCountComboBox->findData(QVariant(1));
    int sampleSize = sampleSizeComboBox->findData(QVariant(16));
    int codec = audioCodecComboBox->findData(QVariant(QString("audio/pcm")));
    int byteOrder = byteOrderComboBox->findData(QVariant(QString("LittleEndian")));
    int sampleType = sampleTypeComboBox->findData(QVariant(QString("SignedInt")));

    // if some properties was not found, try to find "preffered for this device"
    if (sampleRate == -1)
        sampleRate = sampleRateComboBox->findData(QVariant(prefferedFormat.sampleRate()));
    if (channelCount == -1)
        channelCount = channelCountComboBox->findData(QVariant(prefferedFormat.channelCount()));
    if (sampleSize == -1)
        sampleSize = sampleSizeComboBox->findData(QVariant(prefferedFormat.sampleSize()));
    if (codec == -1)
        codec = audioCodecComboBox->findData(QVariant(prefferedFormat.codec()));
    if (byteOrder == -1)
    {
        if (prefferedFormat.byteOrder() == QAudioFormat::BigEndian)
            byteOrder = byteOrderComboBox->findData(QVariant(QString("BigEndian")));
        else
            byteOrder = byteOrderComboBox->findData(QVariant(QString("LittleEndian")));
    }
    if (sampleType == -1)
    {
        QString type = sampleTypeToQString(prefferedFormat.sampleType());
        sampleType = sampleTypeComboBox->findData(QVariant(type));
    }


    // set current indexes for needed format
    sampleRateComboBox->setCurrentIndex(sampleRate);
    channelCountComboBox->setCurrentIndex(channelCount);
    sampleSizeComboBox->setCurrentIndex(sampleSize);
    audioCodecComboBox->setCurrentIndex(codec);
    byteOrderComboBox->setCurrentIndex(byteOrder);
    sampleTypeComboBox->setCurrentIndex(sampleType);
}

void LayoutManager::showBoxes()
{
    if (!labelForSampleRateBox->isHidden())
        hideSpecialLabels();
    deviceBox->show();
    sampleRateComboBox->show();
    channelCountComboBox->show();
    sampleSizeComboBox->show();
    audioCodecComboBox->show();
    byteOrderComboBox->show();
    sampleTypeComboBox->show();
}

void LayoutManager::hideBoxes()
{
    // hide QComboBoxes with audio format options and with Device
    deviceBox->hide();
    sampleRateComboBox->hide();
    channelCountComboBox->hide();
    sampleSizeComboBox->hide();
    audioCodecComboBox->hide();
    byteOrderComboBox->hide();
    sampleTypeComboBox->hide();
}

void LayoutManager::dropBoxes()
{
    hideBoxes();
    deviceBox->show();

    // clear QComboBoxes with audio format options
    sampleRateComboBox->clear();
    channelCountComboBox->clear();
    sampleSizeComboBox->clear();
    audioCodecComboBox->clear();
    byteOrderComboBox->clear();
    sampleTypeComboBox->clear();
}

void LayoutManager::showLabels()
{
    if (!sampleRateComboBox->isHidden())
        hideBoxes();
    labelForDeviceBox->show();
    sampleRateLabel->show();
    channelCountLabel->show();
    sampleSizeLabel->show();
    audioCodecLabel->show();
    byteOrderLabel->show();
    sampleTypeLabel->show();
}

void LayoutManager::hideLabels()
{
    sampleRateLabel->hide();
    channelCountLabel->hide();
    sampleSizeLabel->hide();
    audioCodecLabel->hide();
    byteOrderLabel->hide();
    sampleTypeLabel->hide();
}

void LayoutManager::showSpecialLabels()
{
    // set actual text into special labels
    labelForDeviceBox->setText(deviceBox->currentText());
    labelForSampleRateBox->setText(sampleRateComboBox->currentText());
    labelForChannelCountBox->setText(channelCountComboBox->currentText());
    labelForSampleSizeBox->setText(sampleSizeComboBox->currentText());
    labelForAudioCodecBox->setText(audioCodecComboBox->currentText());
    labelForByteOrderBox->setText(byteOrderComboBox->currentText());
    labelForSampleTypeBox->setText(sampleTypeComboBox->currentText());

    if (!sampleRateComboBox->isHidden())
        hideBoxes();
    // add special labels to mainLayout
    labelForDeviceBox->show();
    labelForSampleRateBox->show();
    labelForChannelCountBox->show();
    labelForSampleSizeBox->show();
    labelForAudioCodecBox->show();
    labelForByteOrderBox->show();
    labelForSampleTypeBox->show();
}

void LayoutManager::hideSpecialLabels()
{
    labelForDeviceBox->hide();
    labelForSampleRateBox->hide();
    labelForChannelCountBox->hide();
    labelForSampleSizeBox->hide();
    labelForAudioCodecBox->hide();
    labelForByteOrderBox->hide();
    labelForSampleTypeBox->hide();
}

void LayoutManager::changeBoxesToLabels()
{
    hideBoxes();
    showSpecialLabels();
}

void LayoutManager::changeLabelsToBoxes()
{
    hideSpecialLabels();
    showBoxes();
}

void LayoutManager::refreshCurrentDeviceInfo()
{
    QAudioDeviceInfo* currentInfo = nullptr;

    // find the right QAudioDeviceInfo
    foreach (const QAudioDeviceInfo &device, QAudioDeviceInfo::availableDevices(audioMode))
    {
        if (device.deviceName() == deviceBox->currentText())
        {
            currentInfo = new QAudioDeviceInfo(device);
            break;
        }
    }

    // if currentDevice was not found, set to him default device
    if (!currentInfo)
    {
        qDebug() << "QAudioDeviceInfo* LayoutManager::getCurrentDeviceInfo(): device was not found :(";
        qDebug() << "Default device was set";
        if (audioMode == QAudio::Mode::AudioInput)
            currentInfo = new QAudioDeviceInfo(QAudioDeviceInfo::defaultInputDevice());
        else
            currentInfo = new QAudioDeviceInfo(QAudioDeviceInfo::defaultOutputDevice());
    }

    // deleting of old device info
    if (currentDeviceInfo)
        delete currentDeviceInfo;

    currentDeviceInfo = currentInfo;
}

void LayoutManager::refreshCurrentAudioFormat()
{
    // read data from boxes
    int sampleRate = sampleRateComboBox->currentText().toInt();
    int channelCount = channelCountComboBox->currentText().toInt();
    int sampleSize = sampleSizeComboBox->currentText().toInt();
    QString audioCodec = audioCodecComboBox->currentText();
    QAudioFormat::Endian byteOrder;
    if (byteOrderComboBox->currentText() == QString("BigEndian"))
        byteOrder = QAudioFormat::Endian::BigEndian;
    else
        byteOrder = QAudioFormat::Endian::LittleEndian;
    QAudioFormat::SampleType sampleType = qStringToSampleType(sampleTypeComboBox->currentText());

    // set format options read from boxes
    currentAudioFormat.setSampleRate(sampleRate);
    currentAudioFormat.setChannelCount(channelCount);
    currentAudioFormat.setSampleSize(sampleSize);
    currentAudioFormat.setCodec(audioCodec);
    currentAudioFormat.setByteOrder(byteOrder);
    currentAudioFormat.setSampleType(sampleType);
}

void LayoutManager::fillDeviceBox()
{
    // Add to currentDeviceBox all avaliavle Audio Input/Output devices
    foreach (const QAudioDeviceInfo &device, QAudioDeviceInfo::availableDevices(audioMode))
        deviceBox->addItem(device.deviceName(), QVariant(device.deviceName()));
}

void LayoutManager::initAllWidgets()
{
    initLabelAndBox();
    initAudioOptionsWidgets();
    fillDeviceBox();
    refreshCurrentAudioFormat();
    initSpecificWidgets();
}

void LayoutManager::initLabelAndBox()
{
    deviceLabel = new QLabel;
    deviceBox = new QComboBox;
    connect(deviceBox, SIGNAL(currentIndexChanged(int)), this, SLOT(showOptionsForCurrentDevice()));

    // add input/output widgets
    mainLayout->addWidget(deviceLabel, 0, 0);
    mainLayout->addWidget(deviceBox, 0, 1);
}

void LayoutManager::showOptionsForCurrentDevice()
{
    dropBoxes(); // drop old boxes with audio options

    refreshCurrentDeviceInfo();

    // fill boxes with options that currentDeviceInfo supports
//    qDebug() << "Filling boxes for " << currentDeviceInfo->deviceName();
    fillBoxes();
    showBoxes();
    setPreferredFormat();
}

void LayoutManager::initAudioOptionsWidgets()
{
    // create labels
    sampleRateLabel = new QLabel("Sample rate (Hz):");
    channelCountLabel = new QLabel("Channels (mono, stereo...):");
    sampleSizeLabel = new QLabel("Sample size (bits):");
    audioCodecLabel = new QLabel("Audio codec:");
    byteOrderLabel = new QLabel("Byte order:");
    sampleTypeLabel = new QLabel("Sample type:");

    // add labels to mainLayout
    mainLayout->addWidget(sampleRateLabel, 1, 0);
    mainLayout->addWidget(channelCountLabel, 2, 0);
    mainLayout->addWidget(sampleSizeLabel, 3, 0);
    mainLayout->addWidget(audioCodecLabel, 4, 0);
    mainLayout->addWidget(byteOrderLabel, 5, 0);
    mainLayout->addWidget(sampleTypeLabel, 6, 0);


    // create boxes
    sampleRateComboBox = new QComboBox;
    channelCountComboBox = new QComboBox;
    sampleSizeComboBox = new QComboBox;
    audioCodecComboBox = new QComboBox;
    byteOrderComboBox = new QComboBox;
    sampleTypeComboBox = new QComboBox;

    // connect box value changed -> refresh current audio format
    connect(sampleRateComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(refreshCurrentAudioFormat()));
    connect(channelCountComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(refreshCurrentAudioFormat()));
    connect(sampleSizeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(refreshCurrentAudioFormat()));
    connect(audioCodecComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(refreshCurrentAudioFormat()));
    connect(byteOrderComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(refreshCurrentAudioFormat()));
    connect(sampleTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(refreshCurrentAudioFormat()));

    // add boxes to mainLayout
    mainLayout->addWidget(sampleRateComboBox, 1, 1);
    mainLayout->addWidget(channelCountComboBox, 2, 1);
    mainLayout->addWidget(sampleSizeComboBox, 3, 1);
    mainLayout->addWidget(audioCodecComboBox, 4, 1);
    mainLayout->addWidget(byteOrderComboBox, 5, 1);
    mainLayout->addWidget(sampleTypeComboBox, 6, 1);

    hideBoxes();

    // create labels for changing of boxes
    labelForDeviceBox = new QLabel;
    labelForSampleRateBox = new QLabel;
    labelForChannelCountBox = new QLabel;
    labelForSampleSizeBox = new QLabel;
    labelForAudioCodecBox = new QLabel;
    labelForByteOrderBox = new QLabel;
    labelForSampleTypeBox = new QLabel;

    // set alignment - center
    labelForDeviceBox->setAlignment(Qt::AlignCenter);
    labelForSampleRateBox->setAlignment(Qt::AlignCenter);
    labelForChannelCountBox->setAlignment(Qt::AlignCenter);
    labelForSampleSizeBox->setAlignment(Qt::AlignCenter);
    labelForAudioCodecBox->setAlignment(Qt::AlignCenter);
    labelForByteOrderBox->setAlignment(Qt::AlignCenter);
    labelForSampleTypeBox->setAlignment(Qt::AlignCenter);

    // add special labels to mainLayout
    mainLayout->addWidget(labelForDeviceBox, 0, 1);
    mainLayout->addWidget(labelForSampleRateBox, 1, 1);
    mainLayout->addWidget(labelForChannelCountBox, 2, 1);
    mainLayout->addWidget(labelForSampleSizeBox, 3, 1);
    mainLayout->addWidget(labelForAudioCodecBox, 4, 1);
    mainLayout->addWidget(labelForByteOrderBox, 5, 1);
    mainLayout->addWidget(labelForSampleTypeBox, 6, 1);
    changeLabelsToBoxes();
}

void LayoutManager::fillBoxes()
{
    // sample rate
    foreach (int sampleRate, currentDeviceInfo->supportedSampleRates())
        sampleRateComboBox->addItem(QString::number(sampleRate), QVariant(sampleRate));

    // channel count
    foreach (int count, currentDeviceInfo->supportedChannelCounts())
        channelCountComboBox->addItem(QString::number(count), QVariant(count));

    // sample size
    foreach (int size, currentDeviceInfo->supportedSampleSizes())
        sampleSizeComboBox->addItem(QString::number(size), QVariant(size));

    // audio codec
    foreach (const QString &codecName, currentDeviceInfo->supportedCodecs())
        audioCodecComboBox->addItem(codecName, QVariant(codecName));

    // byte order
    foreach (const QAudioFormat::Endian &order, currentDeviceInfo->supportedByteOrders())
    {
        if (order == QAudioFormat::Endian::BigEndian)
            byteOrderComboBox->addItem(QString("BigEndian"), QVariant(QString("BigEndian")));
        else
            byteOrderComboBox->addItem(QString("LittleEndian"), QVariant(QString("LittleEndian")));
    }

    // sample type
    foreach (const QAudioFormat::SampleType &sampleType, currentDeviceInfo->supportedSampleTypes())
    {
        QString type = sampleTypeToQString(sampleType);
        sampleTypeComboBox->addItem(type, QVariant(type));
    }
}

QString LayoutManager::sampleTypeToQString(QAudioFormat::SampleType type)
{
    switch (type)
    {
    case QAudioFormat::SampleType::SignedInt:
        return QString("SignedInt");

    case QAudioFormat::SampleType::UnSignedInt:
        return QString("UnSignedInt");

    case QAudioFormat::SampleType::Float:
        return QString("Float");

    default:
        return QString("Unknown");
    }
}

QAudioFormat::SampleType LayoutManager::qStringToSampleType(QString qString)
{
    if (qString == QString("SignedInt"))
        return QAudioFormat::SampleType::SignedInt;
    else if (qString == QString("UnSignedInt"))
        return QAudioFormat::SampleType::UnSignedInt;
    else if (qString == QString("Float"))
        return QAudioFormat::SampleType::Float;
    else
        return QAudioFormat::SampleType::Unknown;
}

void LayoutManager::removeWidgetsFromLayout()
{
    mainLayout->removeWidget(deviceLabel);
    mainLayout->removeWidget(deviceBox);

    mainLayout->removeWidget(sampleRateLabel);
    mainLayout->removeWidget(channelCountLabel);
    mainLayout->removeWidget(sampleSizeLabel);
    mainLayout->removeWidget(audioCodecLabel);
    mainLayout->removeWidget(byteOrderLabel);
    mainLayout->removeWidget(sampleTypeLabel);

    mainLayout->removeWidget(sampleRateComboBox);
    mainLayout->removeWidget(channelCountComboBox);
    mainLayout->removeWidget(sampleSizeComboBox);
    mainLayout->removeWidget(audioCodecComboBox);
    mainLayout->removeWidget(byteOrderComboBox);
    mainLayout->removeWidget(sampleTypeComboBox);

    mainLayout->removeWidget(labelForDeviceBox);
    mainLayout->removeWidget(labelForSampleRateBox);
    mainLayout->removeWidget(labelForChannelCountBox);
    mainLayout->removeWidget(labelForSampleSizeBox);
    mainLayout->removeWidget(labelForAudioCodecBox);
    mainLayout->removeWidget(labelForByteOrderBox);
    mainLayout->removeWidget(labelForSampleTypeBox);
}

void LayoutManager::deleteWidgets()
{
    // deleting of specific for sender/receiver widgets
    delete deviceLabel;
    delete deviceBox;

    // deleting of labels for sound options
    delete sampleRateLabel;
    delete channelCountLabel;
    delete sampleSizeLabel;
    delete audioCodecLabel;
    delete byteOrderLabel;
    delete sampleTypeLabel;


    // deleting of boxes for sound options
    delete sampleRateComboBox;
    delete channelCountComboBox;
    delete sampleSizeComboBox;
    delete audioCodecComboBox;
    delete byteOrderComboBox;
    delete sampleTypeComboBox;

    // deleting of labels for changing of boxes
    delete labelForDeviceBox;
    delete labelForSampleRateBox;
    delete labelForChannelCountBox;
    delete labelForSampleSizeBox;
    delete labelForAudioCodecBox;
    delete labelForByteOrderBox;
    delete labelForSampleTypeBox;

    if (currentDeviceInfo)
        delete currentDeviceInfo;
}


