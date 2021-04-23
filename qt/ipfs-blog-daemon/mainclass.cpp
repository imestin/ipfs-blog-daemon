#include "mainclass.h"
#include <QAction>
#include <QCoreApplication>
#include <QCloseEvent>
#include <QGroupBox>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QDebug>
#include <QTimer>
#include <QProcess>
#include <QObject>
#include <QThread>


MainClass::MainClass() {
	// Create the interface
	createPinningTimesGroupBox();
	createInteractGroupBox();
	createMessagesGroupBox();

	// Create the tray icon
	createActions();
	createTrayIcon();
    setIcon();

	// We might not need all from this
//	connect(showMessageButton, &QAbstractButton::clicked, this, &Window::showMessage);
 //   connect(showIconCheckBox, &QAbstractButton::toggled, trayIcon, &QSystemTrayIcon::setVisible);
    //typedef void (QComboBox::*QComboIntSignal)(int);
 //   connect(iconComboBox, static_cast<QComboIntSignal>(&QComboBox::currentIndexChanged), this, &Window::setIcon);
 //   connect(trayIcon, &QSystemTrayIcon::messageClicked, this, &Window::messageClicked);
 //   connect(trayIcon, &QSystemTrayIcon::activated, this, &Window::iconActivated);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(pinningTimesGroupBox);
    mainLayout->addWidget(interactGroupBox);
    mainLayout->addWidget(messagesGroupBox);
    setLayout(mainLayout);

    // We need to change this so there is no switching in icon
    //iconComboBox->setCurrentIndex(1);
    trayIcon->show();

    setWindowTitle(tr("IPFS Blog Daemon"));
    resize(400, 300);
/*
    // Create the first worker & thread. Later it will be created by the reCreateWorker function
    thread = new QThread;
    worker = new Worker();
    worker->moveToThread(thread);
    thread->start();

    connect(thread, SIGNAL(started()), worker, SLOT(process()));
    workerConnections();*/
}

/*MainClass::~MainClass() {

}*/

void MainClass::setVisible(bool visible) {
	minimizeAction->setEnabled(visible);
    maximizeAction->setEnabled(!isMaximized());
    restoreAction->setEnabled(isMaximized() || !visible);
    QDialog::setVisible(visible);
}

void MainClass::closeEvent(QCloseEvent *event) {
	#ifdef Q_OS_OSX
		if (!event->spontaneous() || !isVisible()) {
        	return;
    	}
	#endif

    if (trayIcon->isVisible()) {
    	QMessageBox::information(this, tr("Systray"), tr("Keep running warning"));
    	hide();
    	event->ignore();
    }
}



void MainClass::setIcon()
{
    QIcon icon = QIcon(":/images/heart.png");
    trayIcon->setIcon(icon);
    setWindowIcon(icon);
}
/*
// most likely we don't need this
void MainClass::iconActivated(QSystemTrayIcon::ActivatedReason reason) {
	switch (reason) {
		case QSystemTrayIcon::Trigger:
		case QSystemTrayIcon::DoubleClick:
			iconComboBox->setCurrentIndex((iconComboBox->currentIndex() + 1) % iconComboBox->count());
			break;
		case QSystemTrayIcon::MiddleClick:
			showMessage();
			break;
		default:
			;
	}
}

// most likely we don't need this
void MainClass::showMessage() {
	showIconCheckBox->setChecked(true);
    QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(typeComboBox->itemData(typeComboBox->currentIndex()).toInt());
    trayIcon->showMessage(titleEdit->text(), bodyEdit->toPlainText(), icon, durationSpinBox->value() * 1000);
}*/



void MainClass::messageClicked() {
	QMessageBox::information(0, tr("Systray"), tr("Pop up message"));
}

void MainClass::createPinningTimesGroupBox() {
	pinningTimesGroupBox = new QGroupBox(tr("Information about pin times"));
	websiteLastLabel = new QLabel(tr("Website last pinned"));
	websiteLastTime = new QLabel(tr("08:08"));
	dbLastLabel = new QLabel(tr("DB last pinned"));
	dbLastTime = new QLabel(tr("08:08"));
	articlesLastLabel = new QLabel(tr("Articles last pinned"));
	articlesLastTime = new QLabel(tr("08:08"));
	nextRefreshLabel = new QLabel(tr("Next refresh"));
	nextRefreshTime = new QLabel(tr("08:08"));

	// Layout could go here (QGridLayout)
}

void MainClass::createInteractGroupBox() {
	interactGroupBox = new QGroupBox(tr("Interactions"));
	changeIntervalLabel = new QLabel(tr("Interval"));
	changeIntervalSpinBox = new QSpinBox();
	changeIntervalSpinBox->setRange(600, 600000);
	changeIntervalSpinBox->setSuffix(" s");
	changeIntervalSpinBox->setValue(600);
	refreshLabel = new QLabel(tr("Refresh now"));
    refreshButton = new QPushButton(tr("This does not even need to be 2 separate entity"));
}

void MainClass::createMessagesGroupBox() {
    messagesGroupBox = new QGroupBox(tr("Messages"));
	messagesLabel = new QLabel(tr("empty message"));
}

void MainClass::createActions() {
	minimizeAction = new QAction(tr("Mi&nimize"), this);
    connect(minimizeAction, &QAction::triggered, this, &QWidget::hide);

    maximizeAction = new QAction(tr("Ma&ximize"), this);
    connect(maximizeAction, &QAction::triggered, this, &QWidget::showMaximized);

    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, &QAction::triggered, this, &QWidget::showNormal);

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
}

void MainClass::createTrayIcon() {
	trayIconMenu = new QMenu(this);
	trayIconMenu->addAction(minimizeAction);
	trayIconMenu->addAction(maximizeAction);
	trayIconMenu->addAction(restoreAction);
	trayIconMenu->addSeparator();
	trayIconMenu->addAction(quitAction);

	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setContextMenu(trayIconMenu);
}


// SIGNAL and SLOT connections for the worker and the main app
void MainClass::workerConnections() {
	connect(timer, SIGNAL(timeout()), worker, SLOT(process()));							// Start    
    connect(worker, SIGNAL(mainSiteFinished()), window(), SLOT(mainSiteDone()));		// Pinning done
    connect(worker, SIGNAL(dbFinished()), window(), SLOT(dbDone()));					// ..
    connect(worker, SIGNAL(articlesFinished()), window(), SLOT(articlesDone()));		// ..
    connect(worker, SIGNAL(finished()), thread, SLOT(quit()));							// End
    connect(worker, SIGNAL(finished()), window(), SLOT(deleteLater()));					// ..
    connect(worker, SIGNAL(error(QString)), window(), SLOT(errorWhilePinning()));		// Error
}


// Update the labels that were created in createPinningTimesGroupBox
void MainClass::updateInterface() {
	// not yet implemented
	// Change label
    websiteLastTime->setText("99:99");
}

void MainClass::reCreateWorker() {
	qDebug() << "Worker finished. Rewrite pointers to nullpointers and re-create them." << endl;
    thread = new QThread;
    worker = new Worker();
    worker->moveToThread(thread);
    thread->start();
    workerConnections();
}

void MainClass::mainSiteDone() {
	qDebug() << "Signal received from worker that pinning the main site recursively is done." << endl;
	// indeed this should change the label, and updateInterface probably should not exist
}

void MainClass::dbDone() {
	qDebug() << "Signal received from worker that pinning the db recursively is done." << endl;
	// indeed this should change the label, and updateInterface probably should not exist
}

void MainClass::articlesDone() {
	qDebug() << "Signal received from worker that pinning the articles recursively is done." << endl;
	// indeed this should change the label, and updateInterface probably should not exist
}

void MainClass::errorWhilePinning() {
	qDebug() << "Error." << endl;
}

/**----------IPFS Functions-----------------------------------------------------------*/
// These should go into a separate file
// --- CONSTRUCTOR ---
Worker::Worker() {
    // you could copy data from constructor arguments to internal variables here.
}

// --- DECONSTRUCTOR ---
Worker::~Worker() {
    // free resources
}

QByteArray Worker::ipfsVersion() {
    QStringList flags;											// This will hold the arguments for the command
    flags.append("--version");
    QProcess *process = new QProcess();
    QByteArray respond;

    process->setProcessChannelMode(QProcess::MergedChannels);
    process->start("ipfs", flags);								// Sending the command to terminal
    if (!process->waitForStarted()) return 0;                   // Wait for it to start (otherwise return)

    while(process->waitForReadyRead())							// Append output to respond variable
        respond.append(process->readAll());

    return respond;
}

QByteArray Worker::ipfsPin(QString which) {
    QStringList flags;											// This will hold the arguments for the command
    flags.append("pin");flags.append("add");flags.append("-r");
    QProcess *process = new QProcess();
    QByteArray respond;
    // These will be dynamic in the future
    QString siteIPNS = "/ipns/k2k4r8klkhk0uh1b3lytws5o724z2a7qiyto4jroedt8yh4huib1mhij";
    QString articlesIPNS = "/ipns/k2k4r8p4uati7s6c981u67ifm7wug2po2rs8a9jlce30l1mgudzzfcz8";
    QString dbIPNS = "/ipns/k2k4r8mqe0akz71ijxrmkvhnmf9da1l5uiuuv854jwundi19kqirh4e4";

    if (which == "site") flags.append(siteIPNS);
    if (which == "articles") flags.append(articlesIPNS);
    if (which == "db") flags.append(dbIPNS);


    process->setProcessChannelMode(QProcess::MergedChannels);
    process->start("ipfs", flags);								// Sending the command to terminal
    if (!process->waitForStarted()) return 0;                   // Wait for it to start (otherwise return)

    while(process->waitForReadyRead())							// Append output to respond variable
        respond.append(process->readAll());

    return respond;
}

void Worker::process() {
    qDebug("Hello World from Maya!");
    QString response;

    response = ipfsVersion();                                   // With this we could test if we have IPFS or not
    if (!response.contains("ipfs version"))
        emit error(response);

    response = ipfsPin("site");                                 // This will pin the main site
    if (response.contains("pinned"))
        emit mainSiteFinished();
    response = ipfsPin("db");                                   // This will pin the DB
    if (response.contains("pinned"))
        emit dbFinished();
    response = ipfsPin("articles");                             // This will pin the articles
    if (response.contains("pinned"))
        emit articlesFinished();

    emit finished();											// Finished without error

}