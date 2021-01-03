#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include "singlescanthread.h"
#include <QDebug>
#include "ctype.h"
#include <QCryptographicHash>
using namespace std;

SingleScanThread::SingleScanThread(QStringList list, QString fileToScan,QString fileLocation, QThread *parent) : QThread(parent)

  ,virusList(list)
  ,passFileToScan(fileToScan)
  ,location(fileLocation)

{
cout << fileLocation.toStdString() << endl;
cout << fileToScan.toStdString() << endl;
QFile file(fileLocation);

{
    QByteArray hashData;

    if (file.open(QIODevice::ReadOnly))
    {
        QByteArray fileData = file.readAll();

        hashData = QCryptographicHash::hash(fileData, QCryptographicHash::Md5); // or QCryptographicHash::Sha1
        qDebug().noquote() << hashData.toHex();  // 0e0c2180dfd784dd84423b00af86e2fc
    }

    std::ifstream lireFichier("C:/Users/TEMP/Desktop/build-NaorisAntiVirus-Desktop_Qt_5_12_10_MinGW_64_bit-Debug/debug/viruslist.txt"); //lire
    if (lireFichier.is_open())
    {

        std::cout << "fichier ouvert" << std::endl;

        std::string ligne;

        while (getline(lireFichier, ligne))
        {
            if (hashData.toStdString() == ligne.c_str())
            {
                std::cout << "OK";
            }
        }
    }
}


}


void SingleScanThread::run() {

    emit scanStart();

    foreach (const QString &str, virusList) {
        if(!stopSingleThread){
            if (str.contains(passFileToScan)) {
                emit infectedFiles(location);
            }
        }
    }
    emit scanComplete();
}



