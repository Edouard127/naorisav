#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logoanimation.h"
#include "logoanimation.h"
#include "aboutdialog.h"
#include "thread.h"
#include "gear.h"
#include "statusone.h"
#include "optionsdialog.h"
#include "warningdialog.h"
#include "abortwarningdialog.h"
#include "singlescanthread.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    setFixedSize(800,525);

    scene = new QGraphicsScene();
    scene->setSceneRect(0,0,641,171);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    threatText = new StatusOne(QPixmap(":images/threatText"));
    threatText->setPos(215,150);
    scene->addItem(threatText);

    mainLogo = new StatusOne(QPixmap(":images/logo2.png"));
    mainLogo->setPos(175,8);
    scene->addItem(mainLogo);
    checkTextOne = false;
    checkTextTwo = false;
    stopMouseMovement = false;

    ui->label->hide();
    ui->label_3->hide();
    ui->install_updates_text->hide();
    ui->progressBar->setValue(0);
    ui->progressBar->setMinimum(0);
    //ui->progressBar->setMaximum(100);

    downloadSignatures();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::startAnimation() {
    ui->label_2->close();
    logo = new LogoAnimation();
    logo->setPos(25, 8);
    scene->addItem(logo);
}

void MainWindow::startGearAnimation() {
    gear = new Gear();
    gear->setPos(330, 60);
    scene->addItem(gear);
}

void MainWindow::on_smartScanButton_clicked() {
    stopMouseMovement = true;
    if(!checkTextOne){
        ui->listWidget->clear();
        QStringList list;
        list << "C:/";
        QFile inputFile("C:/Program Files/Naoris_Antivirus/signatures/viruslist.txt");
        if (inputFile.open(QIODevice::ReadOnly)) {
            QTextStream in(&inputFile);
            QStringList virusList;
            while (!in.atEnd()) {
                QString line = in.readLine();
                virusList << line;
            }
            inputFile.close();
        thread = new Thread(list,virusList);
        connect(thread, &Thread::scanStart, this, &MainWindow::handleScanStart);
        connect(thread, &Thread::scanComplete, this, &MainWindow::handleScanComplete);
        connect(thread, &Thread::infectedFiles, this, &MainWindow::displayInfectedFiles);
        connect(thread, &Thread::finished, thread, &QObject::deleteLater);
        thread->stopThread = false;
        thread->start();

        startAnimation();
        startGearAnimation();

        if(checkTextTwo){
            delete textTwo;
            checkTextTwo = false;
          }
       }
    }
    else{
        warningDialog = new WarningDialog();
        warningDialog->setWindowFlags(Qt::FramelessWindowHint);
        warningDialog->exec();
    }
}

void MainWindow::on_scanSingleFile_clicked() {
    stopMouseMovement = true;
    if(!checkTextOne){
        ui->listWidget->clear();
        QString file = QFileDialog::getOpenFileName(this, tr("Select Directory"), "C:/");
        QFileInfo fi(file);
        QString name = fi.fileName();
        QFile fileLocation(file);

        QByteArray hashDataMd4;
        QByteArray hashDataMd5;
        QByteArray hashDataSha1;
        QByteArray hashDataSha256;
        QStringList hashList;

        //Check to see if file is bigger than 2 GB
        if(fi.size() > 2147483648){
             ui->listWidget->addItem("Error: File size exceeded. Maximum file size is 2GB");
             return;
        }
        if (fileLocation.open(QIODevice::ReadOnly)){
            QByteArray fileData = fileLocation.readAll();           
            hashDataMd5 = QCryptographicHash::hash(fileData, QCryptographicHash::Md5).toHex();
            //hashDataMd4 = QCryptographicHash::hash(fileData, QCryptographicHash::Md4).toHex();
            //hashDataSha1 = QCryptographicHash::hash(fileData, QCryptographicHash::Sha1).toHex();
            //hashDataSha256 = QCryptographicHash::hash(fileData, QCryptographicHash::Sha256).toHex();
            hashList <<  hashDataMd5;
        }
        //There is a viruslist 2 for testing purposes. App is being updated and file written every time currently.
        QFile inputFile("C:/Program Files/Naoris_Antivirus/signatures/viruslist.txt");
        if (inputFile.open(QIODevice::ReadOnly)){
            QTextStream in(&inputFile);
            QStringList virusList;
            while (!in.atEnd()){
                QString line = in.readLine();
                virusList << line;
            }
            inputFile.close();
            singleScanThread = new SingleScanThread(virusList, name, file, hashList);
            connect(singleScanThread, &SingleScanThread::scanStart, this, &MainWindow::handleScanStart);
            connect(singleScanThread, &SingleScanThread::scanComplete, this, &MainWindow::handleScanComplete);
            connect(singleScanThread, &SingleScanThread::infectedFiles, this, &MainWindow::displayInfectedFiles);
            connect(singleScanThread, &SingleScanThread::finished, singleScanThread, &QObject::deleteLater);
            singleScanThread->stopSingleThread = false;
            singleScanThread->start();
            startAnimation();
            startGearAnimation();

            if(checkTextTwo){
                delete textTwo;
                checkTextTwo = false;
            }
        }
    }
    else{
        warningDialog = new WarningDialog();
        warningDialog->setWindowFlags(Qt::FramelessWindowHint);
        warningDialog->exec();
    }
}

void MainWindow::on_scanDirectory_clicked(){
    stopMouseMovement = true;
    if(!checkTextOne){
        QString file = QFileDialog::getExistingDirectory(this, tr("Select Directory"), "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        ui->listWidget->clear();
        QStringList list;
        list<< file;
        if(file.isEmpty()){
            return;
        }
        QFile inputFile("C:/Program Files/Naoris_Antivirus/signatures/viruslist.txt");
        if (inputFile.open(QIODevice::ReadOnly)){
            QTextStream in(&inputFile);
            QStringList virusList;
            while (!in.atEnd()) {
                QString line = in.readLine();
                virusList << line;
            }
            inputFile.close();
        thread = new Thread(list,virusList);
        connect(thread, &Thread::scanStart, this, &MainWindow::handleScanStart);
        connect(thread, &Thread::scanComplete, this, &MainWindow::handleScanComplete);
        connect(thread, &Thread::infectedFiles, this, &MainWindow::displayInfectedFiles);
        connect(thread, &Thread::finished, thread, &QObject::deleteLater);
        thread->stopThread = false;
        thread->start();

        startAnimation();
        startGearAnimation();

        if(checkTextTwo){
            delete textTwo;
            checkTextTwo = false;
        }
      }
    }
    else{
        warningDialog = new WarningDialog();
        warningDialog->setWindowFlags(Qt::FramelessWindowHint);
        warningDialog->exec();
    }
}

void MainWindow::handleScanStart(){
    if(checkTextTwo){
        delete textTwo;
    }
    textOne = new StatusOne(QPixmap(":images/textOne"));
    textOne->setPos(80,90);
    scene->addItem(textOne);
    checkTextOne = true;
}

void MainWindow::handleScanComplete(){
    delete logo;
    delete gear;
    delete textOne;
    checkTextOne = false;
    textTwo = new StatusOne(QPixmap(":images/textTwo"));
    textTwo->setPos(135,120);
    scene->addItem(textTwo);
    checkTextTwo =  true;
    ui->label_2->show();
    if(ui->listWidget->count()== 0){
        ui->listWidget->addItem("No Threats Detected");
    }
}

void MainWindow::stopThread(){
    thread->stopThread = true;
    delete logo;
    delete gear;
    delete textOne;
    checkTextOne = false;
    textTwo = new StatusOne(QPixmap(":images/textTwo"));
    textTwo->setPos(79,92);
    scene->addItem(textTwo);
    checkTextTwo =  true;
    ui->label_2->show();
}

void MainWindow::downloadSignatures(){
    updateThread = new QThread();
    updater = new Updater();
    connect(updater, &Updater::currentProgress, this, &MainWindow::updateProgressBar);
    connect(updater, &Updater::currentProgressFiles, this, &MainWindow::updateProgressBarF);
    connect(updater, &Updater::state, this, &MainWindow::updateStatus);
    updater->moveToThread(updateThread);
    connect(updateThread, &QThread::finished, updater, &QObject::deleteLater);
    updateThread->start();
}

void MainWindow::updateProgressBar(qint64 readBytes, qint64 totalBytes ){
    ui->progressBar->setMaximum(totalBytes);
    ui->progressBar->setValue(readBytes);
}

void MainWindow::updateProgressBarF(int n){
    ui->progressBar->setValue(n);
}

void MainWindow::updateStatus(QString state){
    if(state == "Downloading"){
        if(ui->label_3->isVisible()){
            ui->label_3->hide();
        }
        if(ui->install_updates_text->isVisible()){
            ui->install_updates_text->hide();
        }
        ui->label->show();
    }
    if(state == "Installing"){
        if(ui->label->isVisible()){
            ui->label->hide();
        }
        if(ui->label_3->isVisible()){
            ui->label_3->hide();
        }
        ui->install_updates_text->show();
    }
    if(state == "Complete"){
        if(ui->label->isVisible()){
            ui->label->hide();
        }
        if(ui->install_updates_text->isVisible()){
            ui->install_updates_text->hide();
        }
        ui->label_3->show();

    }
}

void MainWindow::on_abort_clicked(){
    stopMouseMovement = true;
    if(checkTextOne){
        abortWarningDialog = new AbortWarningDialog();
        connect(abortWarningDialog, &AbortWarningDialog::stopThread, this, &MainWindow::stopThread);
        abortWarningDialog->setWindowFlags(Qt::FramelessWindowHint);
        abortWarningDialog->exec();
    }
    else{

        return;
    }
}

void MainWindow::on_removeSelectedFile_clicked(){
    stopMouseMovement = true;
    if(ui->listWidget->count()==0){
        return;
    }
    else if(checkTextOne){
        return;
    }
    else{
        QString fileToRemove = ui->listWidget->currentItem()->text();
        //QFile::setPermissions(ui->listWidget->currentItem()->text(),QFile::ReadOwner|QFile::WriteOwner|QFile::ExeOwner);
        QFile::remove(fileToRemove);
        ui->listWidget->takeItem(ui->listWidget->currentRow());
        ui->listWidget->addItem("Item successfully removed");
    }
}

void MainWindow::on_removeAllFiles_clicked(){
    stopMouseMovement = true;
    if(ui->listWidget->count()==0){
        return;
    }
    else if(checkTextOne){
        return;
    }
    else{
        QString allItemsInOneString;
        int items = ui->listWidget->count();
        for(int i = 0; i < items; i++){
            allItemsInOneString += ui->listWidget->item(i)->text();
        }
        QStringList allItemsAsAList;
        int itemsList = ui->listWidget->count();

        for(int i = 0; i < itemsList; i++){
            allItemsAsAList << ui->listWidget->item(i)->text();
        }
        foreach(QString itemsToDelete,allItemsAsAList){
            QFile::remove(itemsToDelete);
        }
        ui->listWidget->clear();
        ui->listWidget->addItem("All item successfully removed");
    }
}

void MainWindow::on_actionAbout_triggered(){
    stopMouseMovement = true;
    aboutDialog = new AboutDialog();
    aboutDialog->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowSystemMenuHint|Qt::WindowMinimizeButtonHint|Qt::WindowMaximizeButtonHint);
    aboutDialog->exec();
}

void MainWindow::on_actionClose_triggered(){
    qApp->quit();
}

void MainWindow::displayInfectedFiles(QString files){
    QString infectedFiles = files;
    ui->listWidget->addItem(infectedFiles);
}

void MainWindow::on_actionMinimize_triggered(){
   stopMouseMovement = true;
   MainWindow::showMinimized();
}

void MainWindow::on_actionOptions_triggered(){
    stopMouseMovement = true;
    optionsDialog = new OptionsDialog();
    optionsDialog->setWindowFlags(Qt::FramelessWindowHint);
    optionsDialog->exec();
}

void MainWindow::mousePressEvent(QMouseEvent *event){
    stopMouseMovement = false;
    m_nMouseClick_X_Coordinate = event->x();
    m_nMouseClick_Y_Coordinate = event->y();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event){
    if (stopMouseMovement){
        return;
    }else{
    move(event->globalX()-m_nMouseClick_X_Coordinate,event->globalY()-m_nMouseClick_Y_Coordinate);
    }
}

void MainWindow::on_removeSelectedFile_released(){
    stopMouseMovement = true;
}

void MainWindow::on_removeAllFiles_released(){
    stopMouseMovement = true;
}

void MainWindow::on_abort_released(){
    stopMouseMovement = true;
}

void MainWindow::on_smartScanButton_released(){
    stopMouseMovement = true;
}

void MainWindow::on_scanSingleFile_released(){
    stopMouseMovement = true;
}

void MainWindow::on_scanDirectory_released(){
    stopMouseMovement = true;
}

void MainWindow::on_actionOptions_hovered(){
   stopMouseMovement = true;
}

void MainWindow::on_actionAbout_hovered(){
    stopMouseMovement = true;
}

void MainWindow::on_actionMinimize_hovered(){
    stopMouseMovement = true;
}

void MainWindow::on_actionClose_hovered(){
    stopMouseMovement = true;
}

