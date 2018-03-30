#pragma once

#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include <QGridLayout>
#include <QAudio>
#include <QAudioDeviceInfo>
#include <QtDebug>

class LayoutManager : public QObject
{
    Q_OBJECT

public:
    virtual ~LayoutManager();

protected slots:
    /*!
     * \brief changeBoxesToLabels
     * Show labels on mainLayout instead of boxes respectively.
     * When the user starts to send audio, the format should be unavailable for editing.
     */
    void changeBoxesToLabels();
    /*!
     * \brief changeLabelsToBoxes
     * Show boxes on mainLayout instead of labels respectively
     */
    void changeLabelsToBoxes();
    /*!
     * \brief setPreferredFormat
     * Set default format values for this of QComboBoxes
     */
    void setPreferredFormat();
    virtual void connected() = 0;
    virtual void disconnected() = 0;
    virtual void handleStartButtonClicked(){}
    virtual void handleStopButtonClicked(){}
    virtual void handleConnectButtonClicked(){}
    virtual void handleDisconnectButtonClicked(){}
    virtual void handleMuteButtonClicked(){}
    virtual void handleBadConfigure(){}
    virtual void handleGoodConfigure(){}
    virtual void handleStopped(){}
    virtual void handleBufferSizeChanged(int newSize){Q_UNUSED(newSize)}
    virtual void handleProcessedUsec(quint64 usec){Q_UNUSED(usec)}

private slots:
    /*!
     * \brief showOptionsForCurrentDevice
     * Show all options of audio format by Labels and QComboBoxes
     * for the current audio device
     */
    void showOptionsForCurrentDevice();
    void refreshCurrentAudioFormat();
    void refreshCurrentDeviceInfo();

protected:
    void initLabelAndBox();
    /*!
     * \brief initAudioOptionsWidgets
     * Allocation of memory for labels, boxes and special labels for audio options.
     * Adding them to mainLayout.
     */
    void initAudioOptionsWidgets();
    virtual void initSpecificWidgets() = 0;
    void fillDeviceBox();
    void initAllWidgets();

    QGridLayout* mainLayout = nullptr;
    QAudio::Mode audioMode;
    QAudioFormat currentAudioFormat = QAudioFormat();
    QAudioDeviceInfo* currentDeviceInfo = nullptr;

    // current widgets (different for sender and receiver)
    QLabel* deviceLabel = nullptr;
    QComboBox* deviceBox = nullptr;

    QLabel* ipLabel = nullptr;
    QLabel* portLabel = nullptr;
    QLineEdit* ipLineEdit = nullptr;
    QLineEdit* portLineEdit = nullptr;
    QLabel* infoLabel = nullptr;

private:
    void fillBoxes();
    /*!
     * \brief showBoxes
     * Show QComboBoxes with audio options on mainLayout
     */
    void showBoxes();
    /*!
     * \brief hideBoxes
     * Hide boxes for audio format options from mainLayout
     */
    void hideBoxes();
    /*!
     * \brief dropBoxes
     * Hide QComboBoxes for audio format options from mainLayout and clear them
     */
    void dropBoxes();
    /*!
     * \brief showLabels
     * Show QLabels for audio options on mainLayout
     */
    void showLabels();
    /*!
     * \brief hideLabels
     * Hide labels for audio format options from mainLayout
     */
    void hideLabels();
    /*!
     * \brief showSpecialLabels
     * Show special labels to mainLayout on place of boxes respectively
     */
    void showSpecialLabels();
    /*!
     * \brief hideSpecialLabels
     * Hide special labels from mainLayout
     */
    void hideSpecialLabels();
    /*!
     * \brief sampleTypeToQString
     * \param type
     * \return QString for SampleType
     * Adjuvant function, e.g.
     * sampleTypeToQString(QAudioFormat::SampleType::UnSignedInt) == QString("UnSignedInt")
     */
    QString sampleTypeToQString(QAudioFormat::SampleType type);
    /*!
     * \brief qStringToSampleType
     * \param qstring
     * \return SampleType for QString
     * Adjuvant function, e.g.
     * qStringToSampleType(QString("SignedInt")) == QAudioFormat::SampleType::SignedInt
     */
    QAudioFormat::SampleType qStringToSampleType(QString qString);
    /*!
     * \brief removeWidgetsFromLayout
     * Removing of all widgets added to the mainLayout by this manager
     */
    void removeWidgetsFromLayout();
    /*!
     * \brief deleteWidgets
     * Deleting of all widgets added to the mainLayout by this manager.
     * (freeing memory)
     */
    void deleteWidgets();

    // QLabels of options
    QLabel* sampleRateLabel = nullptr;
    QLabel* channelCountLabel = nullptr;
    QLabel* sampleSizeLabel = nullptr;
    QLabel* audioCodecLabel = nullptr;
    QLabel* byteOrderLabel = nullptr;
    QLabel* sampleTypeLabel = nullptr;

    // QComboBoxes of options
    QComboBox* sampleRateComboBox = nullptr;
    QComboBox* channelCountComboBox = nullptr;
    QComboBox* sampleSizeComboBox = nullptr;
    QComboBox* audioCodecComboBox = nullptr;
    QComboBox* byteOrderComboBox = nullptr;
    QComboBox* sampleTypeComboBox = nullptr;

    // QLabels for change current option in boxes to them
    QLabel* labelForDeviceBox = nullptr;
    QLabel* labelForSampleRateBox = nullptr;
    QLabel* labelForChannelCountBox = nullptr;
    QLabel* labelForSampleSizeBox = nullptr;
    QLabel* labelForAudioCodecBox = nullptr;
    QLabel* labelForByteOrderBox = nullptr;
    QLabel* labelForSampleTypeBox = nullptr;
};

