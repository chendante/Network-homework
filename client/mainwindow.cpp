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
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 设置目录显示窗口
    QStringList headerList;
    headerList.append(tr("文件名"));
    headerList.append(tr("创建时间"));
    headerList.append(tr("文件大小"));
    headerList.append(tr("文件路径"));
    ui->tableWidget->setColumnCount(4); //设置列数
    ui->tableWidget->horizontalHeader()->setDefaultSectionSize(150);
    ui->tableWidget->setHorizontalHeaderLabels(headerList);


    //默认绑定8080端口
    int port_num = 8080;
    //当该端口绑定不了时，切换其它端口
    while(!g_UdpSocket.bind(port_num))
    {
        port_num++;
    }
    connect(&g_UdpSocket,SIGNAL(readyRead()),SLOT(GetMessage()));

    //初始化ip地址和端口号
    this->remote_host.setAddress(ui->lineEdit->text());
    this->remote_port = ui->lineEdit_2->text().toInt();

    this->want_count = -1;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::GetMessage()
{
    QByteArray data;

    data.resize(g_UdpSocket.pendingDatagramSize());
    g_UdpSocket.readDatagram(data.data(), data.size());

    QString command;
    QDataStream in(&data,QIODevice::ReadOnly);
    in>>command;
    if(command == "connect success")
    {

    }
    else if (command == "send dir")
    {
        this->Get_dir(&in);
    }
    else if(command == "download file data")
    {
        this->Get_data(&in);
    }
    else if(command == "download file end")
    {
        qDebug()<<command<<endl;
        this->Get_end(&in);
    }
    else if(command == "error")
    {

    }
}

// 向服务器发送指令
void MainWindow::SendMessage(QString command)
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);

    qDebug()<<command;
    out<<command;
    s_UdpSocket.writeDatagram(data,remote_host,remote_port);
}

void MainWindow::on_pushButton_2_clicked()
{
    this->SendMessage("get dir");
}

// 列表某行被双击，询问是否下载
void MainWindow::on_tableWidget_cellDoubleClicked(int row, int column)
{
    QString file_name = ui->tableWidget->item(row, column)->text();
    switch(QMessageBox::question(this,tr("询问"),QString("是否下载 ")+file_name,
                QMessageBox::Ok|QMessageBox::Cancel,QMessageBox::Ok))
    {
    case QMessageBox::Ok:
        if(this->want_count == -1){
            this->download_file_name = file_name;
            this->want_count = 0;
            Download_file();
        }
        else {
            QMessageBox::critical(this, tr("错误"),tr("有文件正在下载"));
        }
        break;
    case QMessageBox::Cancel:
        qDebug()<<"fou ";
        break;
    default:
        break;
    }
}

//发送下载文件请求
void MainWindow::Download_file()
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out<<QString("download file")<<download_file_name<<this->want_count;
    s_UdpSocket.writeDatagram(data,remote_host, remote_port);

    this->download_timer = new QTimer(this);
    connect(download_timer,SIGNAL(timeout()),this,(Download_file()));
    download_timer->start(1000);

    Insert_record("download "+download_file_name+" count:"+want_count);
}

// 按下建立连接按钮
void MainWindow::on_pushButton_clicked()
{
    if(!this->remote_host.setAddress(ui->lineEdit->text()))
    {
        QMessageBox::critical(this, tr("错误"),tr("服务器地址填写错误"));
        return;
    }
    this->remote_port = ui->lineEdit_2->text().toInt();
}

// 用于获取数据的方法
void MainWindow::Get_data(QDataStream* in)
{
    QString file_name;
    *in>>file_name;
    if(file_name == this->download_file_name)
    {
        int count;
        *in>>count;
        // 如果获取的文件片是当前想要的文件片则处理
        if(count == want_count)
        {
            want_count++;
            QByteArray datagram;
            *in>>datagram;
            this->download_file_data.append(datagram.data());
        }
    }
    // 继续请求数据
    this->Download_file();
}

// 用于获取目录的方法
void MainWindow::Get_dir(QDataStream* in)
{
    QJsonDocument readDoc;
    QByteArray json_data;
//    json_data.resize(data.size()-sizeof(int));
    *in>>json_data;
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

// 用于处理接收到结束请求
void MainWindow::Get_end(QDataStream* in)
{
    qDebug()<<"get end"<<endl;
    QString file_name;
    *in>>file_name;
    if(file_name == this->download_file_name)
    {
        int count;
        *in>>count;
        if(count == want_count)
        {
            QFile file(this->download_file_name);
            if(!file.open(QIODevice::WriteOnly)) return;
            file.write(this->download_file_data.data(),download_file_data.size());
            file.close();
            this->want_count = -1;
        }
        else
        {
            this->Download_file();
        }
    }
}

// 上传文件按钮
void MainWindow::on_pushButton_3_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("上传文件"),
                                                    "/",
                                                    tr("任何文件(*.*)"
                                                        ";;文本文件(*.txt)"
                                                        ";;C++文件(*.cpp)"));
    if (fileName.length() != 0) {
        qDebug()<<fileName<<endl;
    }
}

// 添加新记录
void MainWindow::Insert_record(QString record)
{
    ui->textEdit->append(record);
}
