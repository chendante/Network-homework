#ifndef MYFTP_H
#define MYFTP_H

#include "mainwindow.h"

class myftp: public QObject
{
    Q_OBJECT
public:
    myftp(QHostAddress,int,MainWindow*);

    // 判断是否时当前客户
    bool Is_equal(QHostAddress,int);

    void SendMessage(QByteArray data);
    // 发送目录
    void SendDir();
private:
    // 客户端端口信息
    QHostAddress client_host;
    int client_port;
    MainWindow* pp;
    QUdpSocket UdpSocket;

    // 接收的文件数据
    QByteArray receiver_data;
    // 接收文件名
    QString receiver_file_name;
}
#endif // MYFTP_H
