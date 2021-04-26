#ifndef MAINCLASS_H
#define MAINCLASS_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QDialog>

QT_BEGIN_NAMESPACE
class QAction;
class QComboBox;
class QGroupBox;
class QLabel;
class QMenu;
class QPushButton;
class QSpinBox;
QT_END_NAMESPACE




class Worker : public QObject {
	Q_OBJECT

	public:
		Worker();
		~Worker();
		QByteArray ipfsVersion();
		QByteArray ipfsPin(QString which);

	public slots:
		void process();									// second thread that will do the pinning

	signals:
		void finished();
		void mainSiteFinished();
		void dbFinished();
		void articlesFinished();
        void error(QString err);
};


// The main thread
class MainClass : public QDialog {
	Q_OBJECT

	public:
	    MainClass();
	    //~MainClass();									// We don't have a destructor
	    void setVisible(bool visible) Q_DECL_OVERRIDE;

	public slots:
		void reCreateWorker();
		void mainSiteDone();
		void dbDone();
		void articlesDone();


	protected:
		void closeEvent(QCloseEvent * event) Q_DECL_OVERRIDE;

	private slots:

        void setIcon();
        //void iconActivated(QSystemTrayIcon::ActivationReason reason);
        //void showMessage();
		void messageClicked();
		void updateInterface();
        void errorWhilePinning();						// Maybe this should be public or all should be private
        void setInterval();

	private:
		void createPinningTimesGroupBox();
		void createInteractGroupBox();
		void createMessagesGroupBox();
		void createActions();
		void createTrayIcon();
        void createTimer();
        void updateRefreshTime();
        void workerConnections(bool first);

		// GroupBox-1
		QGroupBox *pinningTimesGroupBox;
		QLabel *websiteLastLabel;
		QLabel *websiteLastTime;
		QLabel *dbLastLabel;
		QLabel *dbLastTime;
		QLabel *articlesLastLabel;
		QLabel *articlesLastTime;
		QLabel *nextRefreshLabel;
		QLabel *nextRefreshTime;

		// GroupBox-2
		QGroupBox *interactGroupBox;
		QLabel *changeIntervalLabel;
		QSpinBox *changeIntervalSpinBox;
        QPushButton *setIntervalButton;
		QLabel *refreshLabel;
		QPushButton *refreshButton;

		// GroupBox-3
		QGroupBox *messagesGroupBox;
		QLabel *messagesLabel;

		QAction *minimizeAction;
		QAction *maximizeAction;
		QAction *restoreAction;
		QAction *quitAction;

		QSystemTrayIcon *trayIcon;
		QMenu *trayIconMenu;
		
        int intervalMinutes = 15;

		QTimer *timer;									// Timer for scheduling the updates
		QThread *thread;								// The thread where the IPFS operations will run
		Worker *worker;									// The worker for the second thread

};

#endif // MAINCLASS_H
