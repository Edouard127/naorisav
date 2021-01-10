#ifndef UPDATER_H
#define UPDATER_H

#include <QObject>
#include <QtNetwork>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QFile>
#include <QTimer>
#include <QTextStream>
#include <QDataStream>

class Updater : public QObject
{
    Q_OBJECT

public:
    Updater();

public slots:
    void replyFinished();
    void slotReadyRead();
    void updateDownloadProgress(qint64, qint64);
    void processIncomingData();
    void downloadSignatures();

signals:
    void resultReady(const QString);
    void currentProgress(qint64,qint64);
    void currentProgressFiles(int);
    void dataSize(int);
    void state(QString);

private:
    QNetworkAccessManager *manager;
    QNetworkRequest request;
    QNetworkReply *networkResponse;
    QTimer *fileDownloader;
    int incomingDataSize;         
};

#endif // UPDATER_H
