#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QByteArray>
#include <QDataStream>
#include <QDateTime>
#include <QString>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    connect(&m_timer,SIGNAL(timeout()),this,SLOT(SendMessage()));

//    m_timer.start(1000);
    g_UdpSocket.bind(8081);
    connect(&g_UdpSocket,SIGNAL(readyRead()),SLOT(GetMessage()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SendMessage()
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    QString test("有内鬼");
    out<<QDateTime::currentDateTime()<<test;
    m_UdpSocket.writeDatagram(data,QHostAddress::LocalHost, 8080);
}

void MainWindow::GetMessage()
{
    QByteArray data;
    while (g_UdpSocket.hasPendingDatagrams()) {
        data.resize(g_UdpSocket.pendingDatagramSize());
        g_UdpSocket.readDatagram(data.data(), data.size());
    }
    int Type;
    QString test;
    QDataStream in(&data,QIODevice::ReadOnly);
    in>>Type>>test;
    if(Type == 1)
    {
        this->SendMessage();
    }
    qDebug()<<test;
}
