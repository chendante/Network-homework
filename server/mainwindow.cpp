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
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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
    QString test("使用UDP服务");
    out<<1<<QDateTime::currentDateTime()<<test;
    m_UdpSocket.writeDatagram(data,QHostAddress::LocalHost, 8080);
}

// 发送文件目录
void MainWindow::SendDir()
{
    qDebug()<<"senddir"<<endl;
    QString file_path = this->ui->lineEdit->text();
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    QDir dir(file_path);
    // 当填写目录不存在时，不发送任何内容
    if(!dir.exists())
    {
        this->ui->textEdit->append("wrong path\n");
        return;
    }
    dir.setFilter(QDir::Files|QDir::Hidden|QDir::NoSymLinks);
    dir.setSorting(QDir::Time);
    QFileInfoList d_list = dir.entryInfoList();
    qDebug()<<d_list.size();
    //将d_list 中内容整理为JSON格式
    QJsonArray json_array;
    for (int i = 0; i < d_list.size(); i++)
    {
        QFileInfo fileInfo = d_list.at(i);
        QJsonObject file_json;
        file_json.insert("name",fileInfo.fileName());
        file_json.insert("time",fileInfo.created().toString("yyyy-MM-dd hh:mm:ss"));
        file_json.insert("size",QString::number(fileInfo.size()));
        file_json.insert("path",fileInfo.absoluteFilePath());
        json_array.append(QJsonValue(file_json));
    }
    QJsonDocument json_doc;
    json_doc.setArray(json_array);
    out<<QString("send dir")<<json_doc.toJson(QJsonDocument::Compact);
    m_UdpSocket.writeDatagram(data,QHostAddress::LocalHost, 8080);
}

void MainWindow::GetMessage()
{
    QByteArray data;
    while (g_UdpSocket.hasPendingDatagrams()) {
        data.resize(g_UdpSocket.pendingDatagramSize());
        g_UdpSocket.readDatagram(data.data(), data.size());
    }
    QString command;
    QDataStream in(&data,QIODevice::ReadOnly);
    in>>command;
    if(command == "1")
    {
        this->SendMessage();
    }
    else if(command == "get dir")
    {
        this->SendDir();
    }
    else if(command == "download file")
    {
        int i;
        QString file_name;
        in>>file_name>>i;
        this->SendFile(file_name, i);
    }
    qDebug()<<command;
}

// 按键，更改共享目录地址
void MainWindow::on_pushButton_clicked()
{
    QString dir_url = QFileDialog::getExistingDirectory();
    this->ui->lineEdit->setText(dir_url);
}

// 发送文件
void MainWindow::SendFile(QString file_name, int want_count)
{
    QString file_path = this->ui->lineEdit->text()+file_name;
    QFile file;
    file.setFileName(file_path);
    if(!file.open(QIODevice::ReadOnly)){
        qDebug()<<"wrong" ;
        return;
    }

    int count=0;
    while(!file.atEnd()){
        QByteArray line;
        QDataStream out3(&line, QIODevice::WriteOnly);
        out3<<QString("download file data")<<file_name<<count<<file.read(4000);

        if(count>=want_count)
        {
            g_UdpSocket.writeDatagram(line,QHostAddress::LocalHost,8080);
            return;
        }
        count++;
    }
    QByteArray data2;
    QDataStream out2(&data2, QIODevice::WriteOnly);
    out2<<QString("download file end")<<file_name<<count;
    g_UdpSocket.writeDatagram(data2,QHostAddress::LocalHost,8080);
}
