#include "updater.h"

Updater::Updater(){
    manager = new QNetworkAccessManager(this);

    //Timer to download signatures every ten seconds.
    fileDownloader = new QTimer();
    connect(fileDownloader, SIGNAL(timeout()), this, SLOT(downloadSignatures()));
    fileDownloader->start(10000);
}

void Updater::replyFinished(){
    //qDebug() << networkResponse;
}

//Get the data size
void Updater::slotReadyRead(){
    incomingDataSize = static_cast<int>(networkResponse->size());
}

void Updater::updateDownloadProgress(qint64 bytesRead, qint64 bytesTotal){
    emit state("Downloading");
    emit currentProgress(bytesRead, bytesTotal);
}

//Process the data and write to file
void Updater::processIncomingData(){
    emit dataSize(incomingDataSize);
    QFile file("C:/Program Files/Naoris_Antivirus/signatures/viruslist.txt");
    if (file.open(QIODevice::WriteOnly)){
        int size = 0;
        while(size <= incomingDataSize){
            QTextStream out(&file);
            out << networkResponse->readLine(256);
            //qDebug() <<  networkResponse->readLine(256);
            size += 256;
            emit currentProgressFiles(size);
            emit state("Installing");
            if(size >= incomingDataSize){
                emit currentProgressFiles(incomingDataSize);
                emit state("Complete");
            }
        }
    }else{
        qDebug() << "File Error";
        return;
    }
}

void Updater::downloadSignatures(){
    //These links were returning the html and not the text list on the page
    //request.setUrl(QUrl("http://filesamples.com/samples/document/txt/sample3.txt"));
    //request.setUrl(QUrl("http://manubrial-hour.000webhostapp.com/viruslist.txt"));

    request.setUrl(QUrl("http://25.io/toau/audio/sample.txt"));
    networkResponse = manager->get(request);   
    connect(networkResponse, &QNetworkReply::finished, this, &Updater::processIncomingData);
    connect(networkResponse, &QIODevice::readyRead, this, &Updater::slotReadyRead);
    connect(networkResponse, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(updateDownloadProgress(qint64,qint64)));
}
