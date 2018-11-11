#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QByteArray>
#include <QDateTime>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    g_UdpSocket.bind(8080);
    connect(&g_UdpSocket,SIGNAL(readyRead()),SLOT(GetMessage()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::GetMessage()
{
    QByteArray data;
    while (g_UdpSocket.hasPendingDatagrams()) {
        data.resize(g_UdpSocket.pendingDatagramSize());
        g_UdpSocket.readDatagram(data.data(), data.size());
    }
    QDateTime time;
    QString test;
    QDataStream in(&data,QIODevice::ReadOnly);
    in>>time>>test;
    ui->lineEdit_2->setText(test);
    ui->lineEdit->setText(time.time().toString());
}

void MainWindow::SendMessage(int type)
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    QString test("有内鬼");
    out<<type<<test;
    s_UdpSocket.writeDatagram(data,QHostAddress::LocalHost, 8081);
}

void MainWindow::on_pushButton_clicked()
{
    //获取时间
    this->SendMessage(Ttime);
}
