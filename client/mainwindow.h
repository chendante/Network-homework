#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QFileInfoList>
#include <QJsonArray>
#include <QHostAddress>
#include <QJsonObject>
#include <QTimer>
#include <QDataStream>

#define Ttime 1
#define Tdir  2

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
// 接收UDP数据，并判断命令类型，所有UDP报文都会经过这个函数
    void GetMessage();

// 与用户界面交互的函数
    // 点击获取文件目录按钮
    void on_pushButton_2_clicked();
    // 列表某行被双击，询问是否下载
    void on_tableWidget_cellDoubleClicked(int row, int column);
    // 按下建立连接按钮
    void on_pushButton_clicked();
    // 上传文件按钮
    void on_pushButton_3_clicked();
    // 向屏幕添加新记录
    void Insert_record(QString);

// 以下为向服务器发送命令的函数
    // 向服务器发送指令
    void SendMessage(QString);
    // 发送下载文件请求
    void Download_file();
    // 发送上传文件请求
    void Upload_file();

// 以下为接受到服务端命令后调用的函数
    // 用于获取数据的方法
    void Get_data(QDataStream*);
    // 用于获取目录的方法
    void Get_dir(QDataStream*);
    // 用于处理接收到下载结束请求
    void Get_download_end(QDataStream*);
    // 接收到上传回复
    void Get_receive(QDataStream*);
    // 收到上传文件接收完毕请求
    void Get_upload_end(QDataStream*);

private:
    Ui::MainWindow *ui;
    QUdpSocket UdpSocket;

    // 下载文件名
    QString download_file_name;
    // 下载文件的二进制格式
    QByteArray download_file_data;
    // 下载文件希望的下一个编号
    int download_count;

    // 上传文件地址
    QString upload_file_path;
    // 上传文件希望的下一个编号
    int upload_count;


    // 服务器ip地址
    QHostAddress remote_host;
    // 服务器端口
    int remote_port;

    // 定时器
    QTimer *download_timer;
    QTimer *upload_timer;
};

#endif // MAINWINDOW_H
