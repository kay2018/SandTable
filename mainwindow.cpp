#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    camera=new QCamera(this);
    viewfinder=new QCameraViewfinder(this);
    imageCapture=new QCameraImageCapture(camera);

    ui->ImageView->addWidget(viewfinder);
    ui->ImageCapture->setScaledContents(true);

    camera->setViewfinder(viewfinder);
    camera->start();

    connect(imageCapture, SIGNAL(imageCaptured(int,QImage)), this, SLOT(displayImage(int,QImage)));

    connect(ui->buttonCapture, SIGNAL(clicked()), this, SLOT(captureImage()));
    connect(ui->buttonSave, SIGNAL(clicked()), this, SLOT(saveImage()));
    connect(ui->buttonQuit, SIGNAL(clicked()), qApp, SLOT(quit()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::sendMassage(){}

void MainWindow::readMassage()//读tcp缓冲数据
{
    QByteArray data=tcpSocket->readAll();
    ui->clearLineEdit->setText(QString(data));
    ui->textBrowser->setText(QString(data));
}

void MainWindow::displayError(QAbstractSocket::SocketError)//打印错误信息
{
    QMessageBox::warning (this, tr("Warnning"), tcpSocket->errorString ());
    tcpSocket->close ();
    ui->connnectButton->setEnabled (true);
    ui->disconnectButton->setEnabled (false);
    ui->sendButton->setEnabled (false);
}

void MainWindow::on_sendButton_clicked()//发送数据
{
    QString sendmessage;
    sendmessage = ui->sendLineEdit->text();
    QByteArray data;
    data.append(sendmessage);
    tcpSocket->write(data);
}

void MainWindow::on_clearButton_clicked()//清空接受到的数据
{
    ui->clearLineEdit->clear();
    ui->textBrowser->clear();
}

void MainWindow::on_connnectButton_clicked()//连接TCP端口
{
    flag = false;
    if(tcpSocket) delete tcpSocket;//如果有指向其他空间直接删除
    tcpSocket = new QTcpSocket(this);//申请堆空间有TCP发送和接受操作
    tcpIp = ui->IPLineEdit->text();
    tcpPort = ui->portLineEdit->text();
    if(tcpIp==NULL||tcpPort==NULL)//判断IP和PORT是否为空
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("信息");
        msgBox.setText("IP or PORT is Empty");
        msgBox.exec();
        return;
    }
    tcpSocket->connectToHost(tcpIp,tcpPort.toInt());//连接主机
    connect(tcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,
            SLOT(displayError(QAbstractSocket::SocketError)));//错误连接
    connect(tcpSocket,SIGNAL(connected()),this,
            SLOT(connectUpdata()));//更新连接之后按钮的使能
    connect(tcpSocket,SIGNAL(readyRead()),this,
            SLOT(readMassage()));//读取信息的连接
    ui->connnectButton->setEnabled (false);
    ui->disconnectButton->setEnabled (true);


}

void MainWindow::on_disconnectButton_clicked()//断开连接
{
    tcpSocket->abort();
    delete tcpSocket;
    tcpSocket=NULL;
    disconnectUpdata();
}

void MainWindow::connectUpdata()//连接成功信息以及更新
{
    if(!flag)
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("机器人信息");
        msgBox.setText("机器人握手成功！");
        msgBox.exec();
        ui->connnectButton->setEnabled(false);
        ui->sendButton->setEnabled(true);
        ui->disconnectButton->setEnabled(true);
        ui->IPLineEdit->setEnabled(false);
        ui->portLineEdit->setEnabled(false);
    }
    flag=true;
}

void MainWindow::disconnectUpdata()
{
    ui->connnectButton->setEnabled(true);
    ui->sendButton->setEnabled(false);
    ui->disconnectButton->setEnabled(false);
    ui->IPLineEdit->setEnabled(true);
    ui->portLineEdit->setEnabled(true);
}

/*//////////////////////////////////////////////////////////////*/

void MainWindow::captureImage()
{
    ui->statusBar->showMessage(tr("capturing..."), 1000);
    imageCapture->capture();
}

void MainWindow::displayImage(int , QImage image)
{
    ui->ImageCapture->setPixmap(QPixmap::fromImage(image));

    ui->statusBar->showMessage(tr("capture OK!"), 5000);
}

void MainWindow::saveImage()
{
    QString fileName=QFileDialog::getSaveFileName(this, tr("save file"), QDir::homePath(), tr("jpegfile(*.jpg)"));
    if(fileName.isEmpty()) {
        ui->statusBar->showMessage(tr("save cancel"), 5000);
        return;
    }
    const QPixmap* pixmap=ui->ImageCapture->pixmap();
    if(pixmap) {
        pixmap->save(fileName);
        ui->statusBar->showMessage(tr("save OK"), 5000);
    }
}

