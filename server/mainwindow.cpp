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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_server = new myserver(this);
//    tcpServer = new QTcpServer(this);
//    if(!tcpServer->listen(QHostAddress::Any,24))
//    {
//        qDebug()<<tcpServer->errorString();
//        close();
//    }
//    connect(tcpServer,&QTcpServer::newConnection,this,&MainWindow::NewConnect);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addString(QString str)
{
    qDebug()<<str;
}

void MainWindow::SendMessage()
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    QString test("使用TCP协议");
    out<<1<<QDateTime::currentDateTime()<<test;
    tcpSocket->write(data);
}

void MainWindow::GetMessage()
{
    QByteArray data = tcpSocket->readAll();
    qDebug()<<data;
    int Type;
    QString test;
    QDataStream in(&data,QIODevice::ReadOnly);
    in>>Type>>test;
    if(Type == 1)
    {
        this->SendMessage();
    }
    else if(Type == 2)
    {
        this->SendDir();
    }
    qDebug()<<test;
}

void MainWindow::NewConnect()
{
    tcpSocket = tcpServer->nextPendingConnection();
    connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(GetMessage()));
}
