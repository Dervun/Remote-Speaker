#pragma once

#include <QMainWindow>

#include "LayoutManagement/layoutmanager.h"


namespace Ui
{
    class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

private slots:
    void makeReceiver();
    void makeSender();

private:
    /*!
     * \brief initWelcome
     * Here the welcome buttons are initialized and added to the mainLayout,
     * the connections are created between the buttons presses and their corresponding handlers
     */
    void initWelcome();
    /*!
     * \brief removeWelcomeWidgets
     * Remove welcome widgets when the user makes a choice
     */
    void removeWelcomeWidgets();

    Ui::MainWindow* ui;
    LayoutManager* layoutManager = nullptr;
    QGridLayout* mainLayout = nullptr;

    // welcome wigets
    QLabel* whoAreYouLabel = nullptr;
    QPushButton* receiverButton = nullptr;
    QPushButton* senderButton = nullptr;
};
