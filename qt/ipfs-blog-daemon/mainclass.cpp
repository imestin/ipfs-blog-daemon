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
#include <QDateTime>


MainClass::MainClass() {
    createPinningTimesGroupBox();                                                       // Create the interface
	createInteractGroupBox();
	createMessagesGroupBox();

    createActions();                                                                    // Create the tray icon
	createTrayIcon();
    setIcon();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(pinningTimesGroupBox);
    mainLayout->addWidget(interactGroupBox);
    mainLayout->addWidget(messagesGroupBox);
    setLayout(mainLayout);

    trayIcon->show();
    setWindowTitle(tr("IPFS Blog Daemon"));
    resize(600, 600);                                                                   // Set window size

    connect(setIntervalButton, SIGNAL(clicked()), window(), SLOT(setInterval()));       // Connect the set interval button
    createTimer();                                                                      // Create the timer

    thread = new QThread;                                                               // Create the first worker & thread.
    worker = new Worker();                                                              // Later it will be created by the reCreateWorker function
    worker->moveToThread(thread);
    thread->start();

    workerConnections(true);                                                            // connect() functions for the worker
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
        QMessageBox::information(this, tr("Systray"), tr("The application will keep running in the background. Right click on the icon to open menu with 'Quit' option."));
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

void MainClass::messageClicked() {
	QMessageBox::information(0, tr("Systray"), tr("Pop up message"));
}

void MainClass::createPinningTimesGroupBox() {
	pinningTimesGroupBox = new QGroupBox(tr("Information about pin times"));
	websiteLastLabel = new QLabel(tr("Website last pinned"));
    websiteLastTime = new QLabel(tr("not pinned yet"));
	dbLastLabel = new QLabel(tr("DB last pinned"));
    dbLastTime = new QLabel(tr("not pinned yet"));
	articlesLastLabel = new QLabel(tr("Articles last pinned"));
    articlesLastTime = new QLabel(tr("not pinned yet"));
	nextRefreshLabel = new QLabel(tr("Next refresh"));
    nextRefreshTime = new QLabel(tr("running..."));

    QGridLayout *timesLayout = new QGridLayout;
    timesLayout->addWidget(websiteLastLabel, 0, 0);
    timesLayout->addWidget(dbLastLabel, 1,0);
    timesLayout->addWidget(articlesLastLabel, 2, 0);
    timesLayout->addWidget(nextRefreshLabel, 3, 0);
    timesLayout->addWidget(websiteLastTime, 0, 1);
    timesLayout->addWidget(dbLastTime, 1, 1);
    timesLayout->addWidget(articlesLastTime, 2, 1);
    timesLayout->addWidget(nextRefreshTime, 3, 1);
    pinningTimesGroupBox->setLayout(timesLayout);
}

void MainClass::createInteractGroupBox() {
	interactGroupBox = new QGroupBox(tr("Interactions"));
    changeIntervalLabel = new QLabel(tr("Change refresh interval (will not save)"));
	changeIntervalSpinBox = new QSpinBox();
    changeIntervalSpinBox->setRange(10, 3000);
    changeIntervalSpinBox->setSuffix(" m");
    changeIntervalSpinBox->setValue(intervalMinutes);
    setIntervalButton = new QPushButton(tr("Set"));
    refreshLabel = new QLabel(tr("Do a refresh now"));
    refreshButton = new QPushButton(tr("Refresh now"));

    QGridLayout *interactLayout = new QGridLayout;
    interactLayout->addWidget(changeIntervalLabel, 0, 0);
    interactLayout->addWidget(changeIntervalSpinBox, 0, 1);
    interactLayout->addWidget(setIntervalButton, 0, 2);
    interactLayout->addWidget(refreshLabel, 1, 0);
    interactLayout->addWidget(refreshButton, 1, 1, 1, 2);
    interactGroupBox->setLayout(interactLayout);
}

void MainClass::createMessagesGroupBox() {
    messagesGroupBox = new QGroupBox(tr("Messages"));
	messagesLabel = new QLabel(tr("empty message"));

    QGridLayout *messagesLayout = new QGridLayout;
    messagesLayout->addWidget(messagesLabel, 0, 0);
    messagesGroupBox->setLayout(messagesLayout);
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

void MainClass::createTimer() {
    timer = new QTimer(this);
    timer->start(intervalMinutes*60*1000);
}

void MainClass::setInterval() {
    qDebug() << "hi";
    intervalMinutes = changeIntervalSpinBox->value();
    createTimer();
    updateRefreshTime();
}

void MainClass::updateRefreshTime() {
    QDateTime timeObj = QDateTime::currentDateTime();
    timeObj.setTime_t((timeObj.toMSecsSinceEpoch() + timer->remainingTime())/1000);     // Calculate next refresh time
    nextRefreshTime->setText(timeObj.time().toString());                                // Set next refresh time label
}

// SIGNAL and SLOT connections for the worker and the main app
void MainClass::workerConnections(bool first) {
    if (first) {
        connect(thread, SIGNAL(started()), worker, SLOT(process()));
    } else {
        connect(timer, SIGNAL(timeout()), worker, SLOT(process()));						// Start
    }
    connect(worker, SIGNAL(mainSiteFinished()), window(), SLOT(mainSiteDone()));		// Pinning done
    connect(worker, SIGNAL(dbFinished()), window(), SLOT(dbDone()));					// ..
    connect(worker, SIGNAL(articlesFinished()), window(), SLOT(articlesDone()));		// ..
    connect(worker, SIGNAL(finished()), thread, SLOT(quit()));							// End
    connect(worker, SIGNAL(finished()), window(), SLOT(reCreateWorker()));              // ..
    connect(refreshButton, SIGNAL(clicked()), worker, SLOT(process()));                 // Refresh button
    connect(worker, SIGNAL(processStarted()), window(), SLOT(theProcessStarted()));     // Message when the process started
    connect(worker, SIGNAL(error(QString)), window(), SLOT(errorWhilePinning(QString)));// Error
}


// Update the labels that were created in createPinningTimesGroupBox
void MainClass::updateInterface() {
	// not yet implemented
	// Change label
    // obsolate
    websiteLastTime->setText("99:99");
}

void MainClass::reCreateWorker() {    
    thread = new QThread;
    worker = new Worker();
    worker->moveToThread(thread);
    thread->start();
    workerConnections(false);

    updateRefreshTime();
    QString currentText = messagesLabel->text();
    currentText.append("The pinning process finished. \nYou can click on 'Refresh now' or wait for next scheduled refresh event. \n");
    messagesLabel->setText(currentText);
}

void MainClass::mainSiteDone() {
    QDateTime current = QDateTime::currentDateTime();
    QString currentTime = current.time().toString();
    websiteLastTime->setText(currentTime);
}

void MainClass::dbDone() {
    QDateTime current = QDateTime::currentDateTime();
    QString currentTime = current.time().toString();
    dbLastTime->setText(currentTime);
}

void MainClass::articlesDone() {
    QDateTime current = QDateTime::currentDateTime();
    QString currentTime = current.time().toString();
    articlesLastTime->setText(currentTime);
}

void MainClass::theProcessStarted() {
    messagesLabel->setText(tr("The program started pinning the files from IPFS \n"));
}

void MainClass::errorWhilePinning(QString error) {
    QString currentText = messagesLabel->text();
    if (error == "version") currentText.append("\nError while running `ipfs version`. Check if IPFS is installed \n\n");
    if (error == "site") currentText.append("\nSome error occured while pinning the website frame with 'IPFS pin add -r'. \nMaybe the given IPNS name is wrong or the IPFS network is having problems finding this IPNS name. \n\n");
    if (error == "db") currentText.append("\nSome error occured while pinning the database with 'IPFS pin add -r'. \nMaybe the given IPNS name is wrong or the IPFS network is having problems finding this IPNS name. \n\n");
    if (error == "articles") currentText.append("\nSome error occured while pinning the articles with 'IPFS pin add -r'. \nMaybe the given IPNS name is wrong. or the IPFS network is having problems finding this IPNS name \n\n");
    messagesLabel->setText(currentText);
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
    emit processStarted();
    QString response;

    response = ipfsVersion();                                   // With this we could test if we have IPFS or not
    if (!response.contains("ipfs version")) {
        emit error("version");
        return;
    }


    response = ipfsPin("site");                                 // This will pin the main site
    if (response.contains("pinned")) emit mainSiteFinished();
        else emit error("site");

    response = ipfsPin("db");                                   // This will pin the DB
    if (response.contains("pinned")) emit dbFinished();
        else emit error("db");

    response = ipfsPin("articles");                             // This will pin the articles
    if (response.contains("pinned")) emit articlesFinished();
        else emit error("articles");

    emit finished();											// Finished without error

}
