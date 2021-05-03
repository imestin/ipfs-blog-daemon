/** IPFS functions are here */
#include <QProcess>


Worker::Worker() {
    // nothing happens here
}


Worker::~Worker() {
    // nothing happens here
}


// Main purpose is to check whether IPFS is installed or not
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


// Add new content to IPFS
// Parameter switches ( site | articles | database )
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
