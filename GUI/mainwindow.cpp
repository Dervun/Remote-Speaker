#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "LayoutManagement/sendermanager.h"
#include "LayoutManagement/receivermanager.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("Remote Speaker") + QString(" ") + currentVersion);
    setWindowIcon(QIcon(":/icon.png"));
    mainLayout = new QGridLayout;
    ui->centralwidget->setLayout(mainLayout);
    ui->centralwidget->setMinimumSize(QSize(400, 300));


    initWelcome();
    initTranslatorBox();

    connect(receiverButton, SIGNAL(clicked(bool)), this, SLOT(makeReceiver()));
    connect(senderButton, SIGNAL(clicked(bool)), this, SLOT(makeSender()));
}


MainWindow::~MainWindow()
{
    delete ui;
    delete layoutManager;
    delete mainLayout;
    delete translatorBox;
    if (translator != nullptr)
        delete translator;
}


void MainWindow::makeReceiver()
{
    // remove welcome widgets when the user makes a choice
    removeWelcomeWidgets();

    layoutManager = new ReceiverManager(mainLayout);
    connect(layoutManager, SIGNAL(somebodyConnected()), this, SLOT(somebodyConnected()));
    connect(layoutManager, SIGNAL(somebodyDisonnected()), this, SLOT(somebodyDisonnected()));
}

void MainWindow::makeSender()
{
    // remove welcome widgets when the user makes a choice
    removeWelcomeWidgets();

    layoutManager = new SenderManager(mainLayout);
    connect(layoutManager, SIGNAL(somebodyConnected()), this, SLOT(somebodyConnected()));
    connect(layoutManager, SIGNAL(somebodyDisonnected()), this, SLOT(somebodyDisonnected()));
}

void MainWindow::handleTranslatorChanged()
{
    if (translator)
    {
        QCoreApplication::removeTranslator(translator);
        delete translator;
    }
    translator = new QTranslator;
    if (translator->load(QLocale(nativeToQLocale(translatorBox->currentText())), "apptr", "_", ":/translations", ".qm"))
        QCoreApplication::installTranslator(translator);
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        setWindowTitle(tr("Remote Speaker") + QString(" ") + currentVersion);
        if (whoAreYouLabel)
        {
            whoAreYouLabel->setText(tr("Who are you - receiver or sender?"));
            receiverButton->setText(tr("Receiver"));
            senderButton->setText(tr("Sender"));
        }
    }
    else
        QMainWindow::changeEvent(event);
}

void MainWindow::somebodyConnected()
{
    translatorBox->setEnabled(false);
}

void MainWindow::somebodyDisonnected()
{
    translatorBox->setEnabled(true);
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

    whoAreYouLabel = nullptr;
    receiverButton = nullptr;
    senderButton = nullptr;
}

void MainWindow::initWelcome()
{
    whoAreYouLabel = new QLabel(tr("Who are you - receiver or sender?"));
    whoAreYouLabel->setAlignment(Qt::AlignCenter);
    receiverButton = new QPushButton(tr("Receiver"));
    senderButton = new QPushButton(tr("Sender"));

    // Add welcome widgets
    mainLayout->addWidget(whoAreYouLabel, 0, 0, 1, 2);
    mainLayout->addWidget(receiverButton, 1, 0);
    mainLayout->addWidget(senderButton, 1, 1);
}

void MainWindow::initTranslatorBox()
{
    translatorBox = new QComboBox;
    mainLayout->addWidget(translatorBox, 13, 0);

    translatorBox->addItem(QIcon(":/icons/united-kingdom-flag.png"), QString("English"), QVariant(QString("English")));
    translatorBox->addItem(QIcon(":/icons/russia-flag.png"), QString("Русский"), QVariant(QString("Русский")));
    translatorBox->addItem(QIcon(":/icons/france-flag.png"), QString("Français"), QVariant(QString("Français")));

    connect(translatorBox, SIGNAL(currentIndexChanged(int)), this, SLOT(handleTranslatorChanged()));

    translatorBox->setCurrentIndex(translatorBox->findData(QVariant(qLocaleToNative(QLocale::system().language()))));
}

QLocale::Language MainWindow::nativeToQLocale(const QString &language)
{
    if (language == QString("Русский"))
        return QLocale::Russian;
    if (language == QString("Français"))
        return QLocale::French;
    return QLocale::English;
}

QString MainWindow::qLocaleToNative(const QLocale::Language language)
{
    if (language == QLocale::Russian)
        return QString("Русский");
    if (language == QLocale::French)
        return QString("Français");
    return QString("English");
}
