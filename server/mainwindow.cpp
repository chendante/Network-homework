#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QByteArray>
#include <QDataStream>
#include <QDateTime>
#include <QString>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QTcpSocket>
#include <QThread>
#include "mytcpsocket.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    tcpServer = new QTcpServer(this);
    if(!tcpServer->listen(QHostAddress::Any,25))
    {
        qDebug()<<tcpServer->errorString();
        close();
    }
    connect(tcpServer,&QTcpServer::newConnection,this,&MainWindow::NewConnect);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::GetMessage(QString str)
{
    qDebug()<<str;
    QString now = this->ui->textBrowser->toPlainText();
    now.append("\n");
    now.append(str);
    this->ui->textBrowser->setText(now);
}

void MainWindow::NewConnect()
{
    mytcpsocket* tcp = new mytcpsocket(tcpServer->nextPendingConnection(),this);
//    connect(tcp,SIGNAL(sendString(QString)),this,SLOT(GetMessage(QString)));
}
