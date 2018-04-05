#pragma once

#include <QMainWindow>
#include <QTranslator>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QLocale>

#include "LayoutManagement/layoutmanager.h"


namespace Ui
{
    class MainWindow;
}

/*!
 * \brief The MainWindow class
 * It's main window.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

private slots:
    void makeReceiver();
    void makeSender();
    void handleTranslatorChanged();
    /*!
     * \brief changeEvent
     * \param event
     * Updates the text in widgets according to the new language if it changed,
     * otherwise transfers control to the parent class.
     */
    void changeEvent(QEvent* event);
    void somebodyConnected();
    void somebodyDisonnected();

private:
    /*!
     * \brief initWelcome
     * Here the welcome buttons are initialized and added to the mainLayout,
     * the connections are created between the buttons presses and their corresponding handlers
     */
    void initWelcome();
    /*!
     * \brief initTranslatorBox
     * Here the translatorBox is created and filled, connected signals and slots.
     */
    void initTranslatorBox();
    /*!
     * \brief removeWelcomeWidgets
     * Remove welcome widgets when the user makes a choice
     */
    void removeWelcomeWidgets();
    QLocale::Language nativeToQLocale(const QString &language);
    QString qLocaleToNative(const QLocale::Language language);

    Ui::MainWindow* ui;
    LayoutManager* layoutManager = nullptr; /// Manages by widgets in mainLayout
    QGridLayout* mainLayout = nullptr;

    // welcome wigets
    QLabel* whoAreYouLabel = nullptr;
    QPushButton* receiverButton = nullptr;
    QPushButton* senderButton = nullptr;

    const QString currentVersion = QString("v1.1.0");
    QTranslator* translator = nullptr;
    QComboBox* translatorBox = nullptr;
};
