#include "singlescanthread.h"
#include <QDebug>

SingleScanThread::SingleScanThread(QStringList list, QString fileToScan, QString fileLocation, QString hash, QThread *parent) : QThread(parent), virusList(list), passFileToScan(fileToScan), location(fileLocation), hashValue(hash)
{

}

void SingleScanThread::run() {

    emit scanStart();

    foreach (const QString &str, virusList) {
        if(!stopSingleThread){
            if (str.contains(hashValue)) {
                emit infectedFiles(location);
                emit infectedFiles("Hash: " + hashValue);

            }
        }
    }
    emit scanComplete();
}






