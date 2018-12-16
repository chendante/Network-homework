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
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QStringList headerList;
    headerList.append(tr("文件名"));
    headerList.append(tr("创建时间"));
    headerList.append(tr("文件大小"));
    headerList.append(tr("文件路径"));
    ui->tableWidget->setColumnCount(4); //设置列数
    ui->tableWidget->horizontalHeader()->setDefaultSectionSize(150);
    this->ui->tableWidget->setHorizontalHeaderLabels(headerList);
    //默认绑定8080端口
    int port_num = 8080;
    //当该端口绑定不了时，切换其它端口
    while(!g_UdpSocket.bind(port_num))
    {
        port_num++;
    }
    connect(&g_UdpSocket,SIGNAL(readyRead()),SLOT(GetMessage()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::GetMessage()
{
    QByteArray data;
//    while (g_UdpSocket.hasPendingDatagrams()) {
        data.resize(g_UdpSocket.pendingDatagramSize());
        g_UdpSocket.readDatagram(data.data(), data.size());
//    }
    int Type;
    QDataStream in(&data,QIODevice::ReadOnly);
    in>>Type;
    if(Type == Ttime)
    {
//        in>>time>>test;
//        ui->lineEdit_2->setText(test);
//        ui->lineEdit->setText(time.time().toString());
//        ui->lineEdit_3->setText(time.date().toString());
    }
    else if (Type == Tdir)
    {
        QJsonDocument readDoc;
        QByteArray json_data;
        json_data.resize(data.size()-sizeof(int));
        in>>json_data;
        readDoc= QJsonDocument::fromJson(json_data);
        QJsonArray array_json = readDoc.array();
        ui->tableWidget->setRowCount(array_json.size());
        for(int i = 0; i < array_json.size(); i++)
        {
            QJsonObject file_json(array_json.at(i).toObject());
            this->ui->tableWidget->setItem(i,0,new QTableWidgetItem(file_json.value("name").toString()));
            this->ui->tableWidget->setItem(i,1,new QTableWidgetItem(file_json.value("time").toString()));
            this->ui->tableWidget->setItem(i,2,new QTableWidgetItem(file_json.value("size").toString()));
            this->ui->tableWidget->setItem(i,3,new QTableWidgetItem(file_json.value("path").toString()));
        }
    }
    else if(Type == 4)
    {
        QString file_name;
        qint64 file_size;
        in>>file_name>>file_size;
        if(this->download_file_name == file_name){
            this->download_file_size = file_size;
        }
        this->want_count = 0;
    }
    else if(Type == 5)
    {
        QString file_name;
        in>>file_name;
        if(file_name == this->download_file_name)
        {
            int count;
            in>>count;
            if(count == want_count)
            {
                want_count++;
                QByteArray datagram;
                in>>datagram;
                this->download_file_data.append(datagram.data());
            }
        }
    }
    else if(Type == 6)
    {
        QString file_name;
        in>>file_name;
        if(file_name == this->download_file_name)
        {
            int count;
            in>>count;
            if(count == want_count)
            {
                QFile file(this->download_file_name);
                if(!file.open(QIODevice::WriteOnly)) return;
                file.write(this->download_file_data.data(),download_file_data.size());
                file.close();
            }
        }
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

void MainWindow::on_pushButton_2_clicked()
{
    this->SendMessage(Tdir);
}

// 列表某行被双击，询问是否下载
void MainWindow::on_tableWidget_cellDoubleClicked(int row, int column)
{
    QString file_name = ui->tableWidget->item(row, column)->text();
    switch(QMessageBox::question(this,tr("询问"),QString("是否下载 ")+file_name,
                QMessageBox::Ok|QMessageBox::Cancel,QMessageBox::Ok))
    {
    case QMessageBox::Ok:
        this->download_file_name = file_name;
        Download_file(file_name);
        break;
    case QMessageBox::Cancel:
        qDebug()<<"fou ";
        break;
    default:
        break;
    }
}

//发送下载文件请求
void MainWindow::Download_file(QString file_name)
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out<<3<<file_name;
    s_UdpSocket.writeDatagram(data,QHostAddress::LocalHost, 8081);
}
