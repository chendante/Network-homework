#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QTimer>
#include <QDir>
#include <QFileInfoList>
#include <QFileInfo>
#include <QVector>
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

namespace Ui {
class MainWindow;
}

class myftp;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    // 获取共享目录
    QString GetFilePath();
    // 向屏幕添加记录
    void InsertRecord(QString);

private slots:
    void SendMessage();
    void GetMessage();

// 以下每个函数用于处理每个对应命令
    // 新连接
    void new_connect(QHostAddress ip,int port);
    // 获取目录
    void get_dir(QHostAddress ip,int port);
    // 文件下载
    void download_file(QHostAddress ip,int port,QString file_name,int want_count);
    // 文件数据上传
    void upload_file_data(QHostAddress ip,int port,QDataStream* in);
    void upload_file_end(QHostAddress ip,int port,QDataStream* in);
    
    // 更改共享目录按钮
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QUdpSocket UdpSocket;
    QVector<myftp*> client_list;
};

#endif // MAINWINDOW_H
