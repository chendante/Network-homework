#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QByteArray>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFileInfoList>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonObject>

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
    int Type;
    QDataStream in(&data,QIODevice::ReadOnly);
    in>>Type;
    if(Type == Ttime)
    {
        in>>time>>test;
        ui->lineEdit_2->setText(test);
        ui->lineEdit->setText(time.time().toString());
    }
    else if (Type == Tdir)
    {
        QJsonDocument readDoc;
        QByteArray json_data;
        json_data.resize(data.size()-sizeof(int));
        in>>json_data;
        qDebug()<<json_data;
        readDoc= QJsonDocument::fromJson(json_data);

        qDebug()<<readDoc.toJson();
        QJsonArray array_json = readDoc.array();
        QJsonObject file_json(array_json.at(0).toObject());
        qDebug()<<file_json.value("name").toString();
    }

}

void MainWindow::SendMessage(int type)
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    QString test("有内鬼");
    qDebug()<<type;
    out<<type<<test;
    s_UdpSocket.writeDatagram(data,QHostAddress::LocalHost, 8081);
}

void MainWindow::on_pushButton_clicked()
{
    //获取时间
    this->SendMessage(Ttime);
}

void MainWindow::on_pushButton_2_clicked()
{
    this->SendMessage(Tdir);
}
