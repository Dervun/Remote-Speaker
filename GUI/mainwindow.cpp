#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "LayoutManagement/sendermanager.h"
#include "LayoutManagement/receivermanager.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Remote Speaker v1.0.2");
    setWindowIcon(QIcon(":/icon.png"));
    mainLayout = new QGridLayout;
    ui->centralwidget->setLayout(mainLayout);
    ui->centralwidget->setMinimumSize(QSize(400, 300));

    initWelcome();

    connect(receiverButton, SIGNAL(clicked(bool)), this, SLOT(makeReceiver()));
    connect(senderButton, SIGNAL(clicked(bool)), this, SLOT(makeSender()));
}


MainWindow::~MainWindow()
{
    delete ui;
    delete layoutManager;
    delete mainLayout;
}


void MainWindow::makeReceiver()
{
    // remove welcome widgets when the user makes a choice
    removeWelcomeWidgets();

    layoutManager = new ReceiverManager(mainLayout);
}

void MainWindow::makeSender()
{
    // remove welcome widgets when the user makes a choice
    removeWelcomeWidgets();

    layoutManager = new SenderManager(mainLayout);
}

void MainWindow::removeWelcomeWidgets()
{
    // It's needed here for correct display?
    whoAreYouLabel->hide();
    receiverButton->hide();
    senderButton->hide();

    // removing of widgets added to the mainLayout
    mainLayout->removeWidget(whoAreYouLabel);
    mainLayout->removeWidget(receiverButton);
    mainLayout->removeWidget(senderButton);

    // deleting of welcome widgets
    delete whoAreYouLabel;
    delete receiverButton;
    delete senderButton;
}

void MainWindow::initWelcome()
{
    whoAreYouLabel = new QLabel("Who are you - receiver or sender?");
    whoAreYouLabel->setAlignment(Qt::AlignCenter);
    receiverButton = new QPushButton("Receiver");
    senderButton = new QPushButton("Sender");

    // Add welcome widgets
    mainLayout->addWidget(whoAreYouLabel, 0, 0, 1, 2);
    mainLayout->addWidget(receiverButton, 1, 0);
    mainLayout->addWidget(senderButton, 1, 1);
}

